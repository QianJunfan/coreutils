/* Automated Log Processing program */
/*
* This tool recursively scans directories, extracts descriptions from files,
* and generates a formatted file tree listing all project files.
*
* Currently, alfred uses libfile library for file operations. 
* Additionally, the .alfignore file has been modified to .alf, and the .alf
* file now supports more syntax features:
*             ignoring files with a specific extension
*             adding comments to folders,
*             and even using '#' to add comments for .alf script.
*
*                            Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
*                                                <https://github.com/qianjunfan>
*
*                              Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
*                                             Version Alpha 0.1.1 (Nov 27, 2025)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libfile.h"

#define MAX_PATH 1024
#define MAX_DESC 1024
#define MAX_LINE 1024
#define MAX_PAT 256
#define ROOT_DIR "."
#define README_FILE "./README"
#define IGNORE_FILE "./.alf"
#define INDENT_STR "         "

static char *ignore_pat[MAX_PAT];
static char *dir_desc[MAX_PAT];
static int num_pat = 0;
static int num_dir_desc = 0;

static void free_string_array(char **arr)
{
	if (arr == NULL)
		return;
	for (char **p = arr; *p != NULL; p++)
		free(*p);
	free(arr);
}

static int pat_match(const char *fname)
{
	int i;
	const char *name;
	const char *last_slash;

	if (strcmp(fname, "README") == 0)
		return 1;

	last_slash = strrchr(fname, '/');
	if (last_slash)
		name = last_slash + 1;
	else
		name = fname;

	for (i = 0; i < num_pat; i++) {
		char *pat = ignore_pat[i];
		size_t name_len;
		size_t suffix_len;
		const char *suffix;

		if (!pat || *pat == '\0')
			continue;

		if (pat[strlen(pat) - 1] == '/') {
			if (strcmp(name, pat) == 0 ||
				(strlen(name) > 0 && name[strlen(name) - 1] == '/' &&
				 strncmp(name, pat, strlen(pat) - 1) == 0 &&
				 name[strlen(pat) - 1] == '\0'))
				return 1;
			continue;
		}

		if (strcmp(name, pat) == 0)
			return 1;

		if (pat[0] == '*' && strlen(pat) > 1) {
			suffix = pat + 1;
			suffix_len = strlen(suffix);
			name_len = strlen(name);

			if (name_len >= suffix_len &&
			    strcmp(name + name_len - suffix_len, suffix) == 0)
				return 1;
		}
	}

	return 0;
}

static void load_ignores(void)
{
	FILE *fp;
	char line[MAX_LINE];
	char *trim_line;
	size_t len;
	char *equal_sign;

	fp = fopen(IGNORE_FILE, "r");
	if (!fp) {
		printf("Warning: Could not open configuration file %s. Proceeding without ignore list and dir descriptions.\n",
		       IGNORE_FILE);
		return;
	}

	while (fgets(line, sizeof(line), fp)) {
		trim_line = line;

		while (*trim_line == ' ' || *trim_line == '\t')
			trim_line++;

		len = strlen(trim_line);
		while (len > 0 && (trim_line[len - 1] == '\n' ||
				   trim_line[len - 1] == ' ' ||
				   trim_line[len - 1] == '\t'))
			trim_line[--len] = '\0';

		if (*trim_line == '\0' || *trim_line == '#')
			continue;

		equal_sign = strchr(trim_line, '=');

		if (equal_sign) {
			if (num_dir_desc < MAX_PAT) {
				dir_desc[num_dir_desc] = strdup(trim_line);
				if (dir_desc[num_dir_desc])
					num_dir_desc++;
			}
		} else {
			if (num_pat < MAX_PAT) {
				ignore_pat[num_pat] = strdup(trim_line);
				if (ignore_pat[num_pat])
					num_pat++;
			}
		}
	}

	fclose(fp);
}

static void free_pat(void)
{
	int i;

	for (i = 0; i < num_pat; i++)
		free(ignore_pat[i]);

	for (i = 0; i < num_dir_desc; i++)
		free(dir_desc[i]);
}

static char *get_dir_desc(const char *dir_name)
{
	int i;
	char *desc = NULL;
	char *equal_sign;
	char *dir_name_end;
	char name_buf[MAX_PATH];
	const char *name_ptr;

	name_ptr = strrchr(dir_name, '/');
	name_ptr = name_ptr ? name_ptr + 1 : dir_name;

	for (i = 0; i < num_dir_desc; i++) {
		if (!dir_desc[i])
			continue;

		equal_sign = strchr(dir_desc[i], '=');
		if (!equal_sign)
			continue;

		strncpy(name_buf, dir_desc[i], equal_sign - dir_desc[i]);
		name_buf[equal_sign - dir_desc[i]] = '\0';

		dir_name_end = name_buf + strlen(name_buf) - 1;
		while (dir_name_end >= name_buf && (*dir_name_end == ' ' || *dir_name_end == '\t')) {
			*dir_name_end = '\0';
			dir_name_end--;
		}

		if (strcmp(name_ptr, name_buf) == 0) {
			char *desc_start = equal_sign + 1;
			while (*desc_start == ' ' || *desc_start == '\t')
				desc_start++;

			desc = strdup(desc_start);
			break;
		}
	}

	return desc;
}

static char *get_file_desc(const char *fpath)
{
	char **lines = NULL;
	char *desc = NULL;
	char *trim_line;
	char *start_cmt;
	char *end_cmt;
	char *start;
	size_t len;
	int found = 0;
	int i;

	lines = file_ls(fpath);
	if (!lines)
		return NULL;

	for (i = 0; lines[i] != NULL; i++) {
		trim_line = lines[i];

		while (*trim_line == ' ' || *trim_line == '\t')
			trim_line++;

		if (*trim_line == '\0')
			continue;

		if (*trim_line == '#')
			continue;

		found = 1;
		break;
	}

	if (!found)
		goto cleanup;

	start_cmt = strstr(trim_line, "/*");
	end_cmt = strstr(trim_line, "*/");

	if (start_cmt && end_cmt && end_cmt > start_cmt) {
		start = start_cmt + 2;
		len = end_cmt - start;

		if (len >= MAX_DESC)
			len = MAX_DESC - 1;

		desc = malloc(len + 1);
		if (!desc)
			goto cleanup;

		strncpy(desc, start, len);
		desc[len] = '\0';
	} else {
		desc = strdup(trim_line);
		if (!desc)
			goto cleanup;
	}

cleanup:
	free_string_array(lines);

	if (desc) {
		len = strlen(desc);

		while (len > 0 && (desc[len - 1] == '\n' ||
				   desc[len - 1] == ' ' ||
				   desc[len - 1] == '\t'))
			desc[--len] = '\0';

		start = desc;
		while (*start == ' ' || *start == '\t')
			start++;

		if (start != desc)
			memmove(desc, start, strlen(start) + 1);

		if (strlen(desc) >= MAX_DESC)
			desc[MAX_DESC - 1] = '\0';
	}

	return desc;
}

static void write_desc(FILE *fp, const char *desc)
{
	fprintf(fp, "- %s\n", desc);
	printf("- %s\n", desc);
}

static void write_entry(FILE *fp, const char *name, const char *desc,
			     int max_len)
{
	int padding;
	int i;
	const char *final_desc;

	padding = max_len - (int)strlen(name);
	final_desc = desc ? desc : "????";

	fprintf(fp, INDENT_STR "%s", name);
	printf(INDENT_STR "%s", name);

	for (i = 0; i < padding + 1; i++) {
		fputc(' ', fp);
		fputc(' ', stdout);
	}

	write_desc(fp, final_desc);
}

static int find_max_len(const char *path)
{
	DIR *dir;
	struct dirent *dent;
	struct stat st;
	char child_path[MAX_PATH];
	const char *name;
	int max_len = 0;
	int sub_len;
	int len;

	dir = opendir(path);
	if (!dir)
		return 0;

	while ((dent = readdir(dir)) != NULL) {
		name = dent->d_name;

		if (name[0] == '.')
			continue;

		if (pat_match(name))
			continue;

		snprintf(child_path, MAX_PATH, "%s/%s", path, name);

		if (stat(child_path, &st) < 0)
			continue;

		if (S_ISREG(st.st_mode)) {
			len = (int)strlen(name);
			if (len > max_len)
				max_len = len;
		} else if (S_ISDIR(st.st_mode)) {
			sub_len = find_max_len(child_path);
			if (sub_len > max_len)
				max_len = sub_len;
		}
	}
	closedir(dir);
	return max_len;
}

static void scan_dir(FILE *fp, const char *path, const char *prefix,
			   int max_len)
{
	DIR *dir;
	struct dirent *dent;
	struct stat st;
	char child_path[MAX_PATH];
	char next_prefix[MAX_PATH];
	const char *name;
	char *desc;

	dir = opendir(path);
	if (!dir)
		return;

	while ((dent = readdir(dir)) != NULL) {
		name = dent->d_name;

		if (name[0] == '.')
			continue;

		if (pat_match(name))
			continue;

		snprintf(child_path, MAX_PATH, "%s/%s", path, name);

		if (stat(child_path, &st) < 0)
			continue;

		if (S_ISREG(st.st_mode)) {
			desc = get_file_desc(child_path);
			write_entry(fp, name, desc, max_len);
			if (desc)
				free(desc);
		}
	}

	rewinddir(dir);
	while ((dent = readdir(dir)) != NULL) {
		name = dent->d_name;

		if (name[0] == '.')
			continue;

		if (pat_match(name))
			continue;

		snprintf(child_path, MAX_PATH, "%s/%s", path, name);

		if (stat(child_path, &st) < 0)
			continue;

		if (S_ISDIR(st.st_mode)) {
			snprintf(next_prefix, MAX_PATH, "%s%s/",
				 prefix, name);

			desc = get_dir_desc(child_path);

			fprintf(fp, "\n%s", next_prefix);
			printf("\n%s", next_prefix);

			if (desc) {
				fprintf(fp, " - %s", desc);
				printf(" - %s", desc);
				free(desc);
			}

			fputc('\n', fp);
			fputc('\n', stdout);


			scan_dir(fp, child_path, next_prefix, max_len);
		}
	}
	closedir(dir);
}

int main(void)
{
	FILE *readme_fp;
	int exit_code = 0;
	int global_max_len;

	load_ignores();

	readme_fp = fopen(README_FILE, "w");
	if (!readme_fp) {
		perror("Failed to open ../README for writing");
		free_pat();
		return 1;
	}

	fprintf(readme_fp, "./\n");
	printf("./\n");

	global_max_len = find_max_len(ROOT_DIR);

	scan_dir(readme_fp, ROOT_DIR, "", global_max_len);

	if (fclose(readme_fp) != 0) {
		perror("Failed to close ../README");
		exit_code = 1;
	}

	if (exit_code == 0)
		printf("\n" INDENT_STR
		       "> Successfully generated and wrote file information to %s\n",
		       README_FILE);

	free_pat();

	return exit_code;
}
