#include "../libfile.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

static void free_string_array(char **arr)
{
        if (arr == NULL)
                return;
        for (char **p = arr; *p != NULL; p++)
                free(*p);
        free(arr);
}

static void trim_newline(char *s)
{
        size_t len = strlen(s);

        if (len > 0 && s[len - 1] == '\n')
                s[len - 1] = '\0';
}

uint64_t file_le(const char *file_path)
{
        FILE *fp;
        uint64_t count = 0;
        char *line = NULL; 
        size_t len = 0;   

        fp = fopen(file_path, "r");
        if (!fp)
                return 0;
        
        while (getline(&line, &len, fp) != -1)
                count++;

        if (line)
                free(line);
        
        fclose(fp);
        
        return count;
}

char **file_ls(const char *file_path)
{
        uint64_t line_count;
        FILE *fp;
        char **lines;
        char *line_buf = NULL;
        size_t len = 0;
        uint64_t i = 0;

        line_count = file_le(file_path);
        if (!line_count && !file_ex(file_path))
                return NULL;
        
        fp = fopen(file_path, "r");
        if (!fp)
                return NULL;

        lines = malloc((line_count + 1) * sizeof(char *));
        if (!lines) {
                fclose(fp);
                return NULL;
        }

        fseek(fp, 0, SEEK_SET);

        while (i < line_count && getline(&line_buf, &len, fp) != -1) {
                trim_newline(line_buf);
                
                lines[i] = strdup(line_buf);
                if (!lines[i]) {
                        free_string_array(lines);
                        if (line_buf)
                                free(line_buf);
                        fclose(fp);
                        return NULL;
                }
                i++;
        }
        
        if (line_buf)
                free(line_buf);
        
        lines[i] = NULL; 
        fclose(fp);
        
        return lines;
}

char *file_li(const char *file_path, uint64_t cnt)
{
        FILE *fp;
        char *line_buf = NULL;
        size_t len = 0;
        uint64_t current_line = 0;
        char *result = NULL;

        if (!cnt)
                return NULL;

        fp = fopen(file_path, "r");
        if (!fp)
                return NULL;

        while (getline(&line_buf, &len, fp) != -1) {
                current_line++;
                if (current_line == cnt) {
                        trim_newline(line_buf);
                        result = strdup(line_buf);
                        break;
                }
        }
        
        if (line_buf)
                free(line_buf);
        
        fclose(fp);
        
        return result; 
}

bool file_ex(const char *file_path)
{
        struct stat st;

        if (stat(file_path, &st) == 0)
                return S_ISREG(st.st_mode); 

        return false;
}

bool dir_ex(const char *dict_path)
{
        struct stat st;

        if (stat(dict_path, &st) == 0)
                return S_ISDIR(st.st_mode);

        return false;
}

uint64_t dir_le(const char *dict_path)
{
        DIR *dir;
        uint64_t count = 0;

        dir = opendir(dict_path);
        if (!dir)
                return 0;

        while (readdir(dir) != NULL)
                count++;

        closedir(dir);

        return count;
}

char **dir_ls(const char *dict_path)
{
        uint64_t count;
        DIR *dir;
        char **entries;
        struct dirent *entry;
        uint64_t i = 0;
        
        count = dir_le(dict_path);
        if (!count && !dir_ex(dict_path))
                return NULL;

        dir = opendir(dict_path);
        if (!dir)
                return NULL;

        entries = malloc((count + 1) * sizeof(char *));
        if (!entries) {
                closedir(dir);
                return NULL;
        }

        rewinddir(dir);

        while (i < count && (entry = readdir(dir)) != NULL) {
                entries[i] = strdup(entry->d_name);
                if (!entries[i]) {
                        free_string_array(entries);
                        closedir(dir);
                        return NULL;
                }
                i++;
        }

        entries[i] = NULL; 
        closedir(dir);

        return entries;
}
