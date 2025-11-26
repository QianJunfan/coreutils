#include "../libtio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#define MAX_LINES 100
#define MAX_LINE_LEN 256

enum {
	KEY_UP		= 0x101,
	KEY_DOWN	= 0x102,
	KEY_RIGHT	= 0x103,
	KEY_LEFT	= 0x104,
	KEY_BACKSPACE	= 0x7f,
};

static struct termios t_orig;
static struct termios raw_termios;

static void tty_raw(void)
{
	struct termios raw;

	if (tcgetattr(STDIN_FILENO, &raw_termios) == -1)
		exit(EXIT_FAILURE);

	raw = raw_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	raw.c_iflag &= ~(IXON | ICRNL);
	raw.c_oflag &= ~(OPOST);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		exit(EXIT_FAILURE);
}

static void tty_restore(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios);
}

char *io_readl(void)
{
	int c;
	size_t len = 0;
	size_t bsz = 128;
	char *buf;
	char *tmp;

	buf = malloc(bsz);
	if (!buf)
		return NULL;

	while ((c = getchar()) != '\n' && c != EOF) {
		if (len >= bsz - 1) {
			bsz *= 2;
			tmp = realloc(buf, bsz);
			if (!tmp) {
				free(buf);
				return NULL;
			}
			buf = tmp;
		}
		buf[len] = (char)c;
		len++;
	}

	if (len == 0 && c == EOF) {
		free(buf);
		return NULL;
	}

	buf[len] = '\0';
	return buf;
}

static void redraw_line(char *line, int pos)
{
	int len = strlen(line);

	printf("\r\x1b[2K%s", line);
	if (pos < len)
		printf("\x1b[%dD", len - pos);
	fflush(stdout);
}

static int read_key(void)
{
	int nread;
	char c;
	char seq[3];

	nread = read(STDIN_FILENO, &c, 1);
	if (nread == 0)
		return 0;
	if (nread < 0)
		return -1;

	if (c != '\x1b')
		return c;

	if (read(STDIN_FILENO, &seq[0], 1) != 1)
		return '\x1b';
	if (read(STDIN_FILENO, &seq[1], 1) != 1)
		return '\x1b';

	if (seq[0] == '[') {
		switch (seq[1]) {
		case 'A': return KEY_UP;
		case 'B': return KEY_DOWN;
		case 'C': return KEY_RIGHT;
		case 'D': return KEY_LEFT;
		}
	}

	return '\x1b';
}

static void line_delete_char(char *line, int *pos)
{
	int len = strlen(line);

	if (*pos > 0) {
		memmove(line + *pos - 1, line + *pos, len - *pos + 1);
		(*pos)--;
		redraw_line(line, *pos);
	}
}

static void line_insert_char(char *line, int *pos, char c)
{
	int len = strlen(line);

	if (len >= MAX_LINE_LEN - 1)
		return;

	memmove(line + *pos + 1, line + *pos, len - *pos + 1);
	line[*pos] = c;
	(*pos)++;
	redraw_line(line, *pos);
}

static void free_lines(char **lines, int count)
{
	int i;

	for (i = 0; i < count; i++)
		free(lines[i]);
	free(lines);
}

char **io_readm(int *cnt, char terminator)
{
	char **lines;
	char *curr;
	int n_lines = 1;
	int cur_idx = 0;
	int cur_pos = 0;
	int c, i;

	tty_raw();
	tcflush(STDIN_FILENO, TCIFLUSH);

	lines = calloc(MAX_LINES, sizeof(char *));
	if (!lines)
		goto err;

	lines[0] = calloc(MAX_LINE_LEN, sizeof(char));
	if (!lines[0]) {
		free(lines);
		goto err;
	}

	*cnt = 0;

	while (1) {
		c = read_key();
		if (c == 0)
			continue;
		if (c < 0) {
			if (errno == EINTR)
				continue;
			goto err_cleanup;
		}

		curr = lines[cur_idx];

		if (c == terminator) {
			if (n_lines > 0 && strlen(lines[n_lines - 1]) == 0 && n_lines > 1) {
				free(lines[n_lines - 1]);
				lines[n_lines - 1] = NULL;
				n_lines--;
			} else if (n_lines == 1 && strlen(lines[0]) == 0) {
				free(lines[0]);
				lines[0] = NULL;
				n_lines = 0;
			}
			
			printf("\x1b");
			printf("c");
			printf("\x1b[g");
			
			printf("\r\x1b[2K");
			printf("\r\n");
			
			tty_restore();

			*cnt = n_lines;
			fflush(stdout);
			return lines;
		}

		switch (c) {
		case '\r':
		case '\n':
			if (n_lines >= MAX_LINES)
				goto err_cleanup;

			printf("\r\n");
			fflush(stdout);
			cur_idx++;

			if (cur_idx >= n_lines) {
				lines[n_lines] = calloc(MAX_LINE_LEN, sizeof(char));
				if (!lines[n_lines])
					goto err_cleanup;
				n_lines++;
			}
			cur_pos = 0;
			break;

		case KEY_BACKSPACE:
		case '\b':
			if (cur_pos > 0) {
				line_delete_char(curr, &cur_pos);
			} else if (cur_idx > 0) {
				printf("\x1b[A\x1b[2K");
				free(lines[cur_idx]);

				for (i = cur_idx; i < n_lines - 1; i++)
					lines[i] = lines[i + 1];
				lines[n_lines - 1] = NULL;
				n_lines--;

				cur_idx--;
				curr = lines[cur_idx];
				cur_pos = strlen(curr);
				redraw_line(curr, cur_pos);
			}
			break;

		case KEY_UP:
			if (cur_idx > 0) {
				printf("\x1b[A");
				cur_idx--;
				curr = lines[cur_idx];
				if (cur_pos > (int)strlen(curr))
					cur_pos = strlen(curr);
				redraw_line(curr, cur_pos);
			}
			break;

		case KEY_DOWN:
			if (cur_idx < n_lines - 1) {
				printf("\x1b[B");
				cur_idx++;
				curr = lines[cur_idx];
				if (cur_pos > (int)strlen(curr))
					cur_pos = strlen(curr);
				redraw_line(curr, cur_pos);
			}
			break;

		case KEY_RIGHT:
			if (cur_pos < (int)strlen(curr)) {
				printf("\x1b[C");
				cur_pos++;
			}
			break;

		case KEY_LEFT:
			if (cur_pos > 0) {
				printf("\x1b[D");
				cur_pos--;
			}
			break;

		default:
			if (c >= 32 && c < 127)
				line_insert_char(curr, &cur_pos, (char)c);
			break;
		}
	}

err_cleanup:
	printf("\x1b");
	printf("c");
	printf("\x1b[g");
	
	printf("\r\x1b[2K");
	printf("\r\n");
	
	tty_restore();

	free_lines(lines, n_lines);
err:
	*cnt = 0;
	return NULL;
}