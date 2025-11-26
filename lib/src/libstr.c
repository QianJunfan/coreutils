#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../libstr.h"

void str_arr_free(char **res, int count)
{
        int i;

        if (!res)
                return;

        for (i = 0; i < count; ++i) {
                if (res[i])
                        free(res[i]);
        }
        free(res);
}

char **strsplit(const char *s, char delim, int *cnt)
{
        char **res = NULL;
        const char *p = s;
        const char *st = s;
        int tok = 0;
        int cap = 4;
        char **new;

        if (!s || !cnt) {
                if (cnt) *cnt = 0;
                return NULL;
        }

        res = malloc(cap * sizeof(char*));
        if (!res) {
                *cnt = 0;
                return NULL;
        }

        while (1) {
                if (*p == delim || *p == '\0') {
                        size_t len = p - st;

                        if (len > 0) {
                                if (tok + 1 >= cap) {
                                        cap *= 2;
                                        new = realloc(res, cap * sizeof(char*));
                                        if (!new) {
                                                str_arr_free(res, tok);
                                                *cnt = 0;
                                                return NULL;
                                        }
                                        res = new;
                                }

                                res[tok] = malloc(len + 1);
                                if (!res[tok]) {
                                        str_arr_free(res, tok);
                                        *cnt = 0;
                                        return NULL;
                                }
                                strncpy(res[tok], st, len);
                                res[tok][len] = '\0';
                                tok++;
                        }

                        if (*p == '\0')
                                break;

                        st = p + 1;
                }
                
                p++;
        }

        new = realloc(res, (tok + 1) * sizeof(char*));
        if (!new) {
                *cnt = tok;
                return res;
        }
        res = new;
        res[tok] = NULL;
        *cnt = tok;

        return res;
}

char *strltrim(char *s)
{
        size_t len = strlen(s);
        size_t i;

        if (!len)
                return s;

        for (i = 0; i < len && isspace((unsigned char)s[i]); ++i);

        if (i > 0)
                memmove(s, s + i, len - i + 1);

        return s;
}

char *strrtrim(char *s)
{
        size_t len = strlen(s);

        if (!len)
                return s;

        while (len > 0 && isspace((unsigned char)s[len - 1]))
                len--;

        s[len] = '\0';
        return s;
}

char *strtrim(char *s)
{
        strrtrim(s);
        strltrim(s);
        return s;
}

char *strtolower(char *s)
{
        char *p = s;

        while (*p) {
                *p = tolower((unsigned char)*p);
                p++;
        }
        return s;
}

char *strtoupper(char *s)
{
        char *p = s;

        while (*p) {
                *p = toupper((unsigned char)*p);
                p++;
        }
        return s;
}

int streqi(const char *s1, const char *s2)
{
        int c1;
        int c2;

        if (!s1 || !s2)
                return 0;

        do {
                c1 = tolower((unsigned char)*s1++);
                c2 = tolower((unsigned char)*s2++);
                if (c1 != c2)
                        return c1 - c2;
        } while (c1);

        return 0;
}

int strstarts(const char *s, const char *start)
{
        return !strncmp(s, start, strlen(start));
}

int strends(const char *s, const char *end)
{
        size_t slen = strlen(s);
        size_t elen = strlen(end);

        if (slen < elen)
                return 0;

        return !strcmp(s + slen - elen, end);
}

char *strjoin(char **arr, int count, const char *delim)
{
        size_t dlen = 0;
        size_t total_len = 0;
        char *res;
        char *p;
        int i;

        if (!arr || count <= 0)
                return strdup("");

        if (delim)
                dlen = strlen(delim);

        for (i = 0; i < count; ++i) {
                if (arr[i])
                        total_len += strlen(arr[i]);
        }
        
        if (count > 0)
                total_len += dlen * (count - 1);

        res = malloc(total_len + 1);
        if (!res)
                return NULL;

        p = res;
        for (i = 0; i < count; ++i) {
                size_t clen = 0;
                if (arr[i]) {
                        clen = strlen(arr[i]);
                        memcpy(p, arr[i], clen);
                        p += clen;
                }

                if (i < count - 1 && dlen > 0) {
                        memcpy(p, delim, dlen);
                        p += dlen;
                }
        }
        *p = '\0';

        return res;
}

char *strreplace(const char *s, const char *old, const char *new)
{
        const char *p = s;
        const char *found;
        size_t oldlen = strlen(old);
        size_t newlen = strlen(new);
        size_t slen = strlen(s);
        size_t count = 0;
        size_t reslen;
        char *res;
        char *rp;

        if (!s || !old || !new || oldlen == 0)
                return strdup(s);

        while ((found = strstr(p, old))) {
                count++;
                p = found + oldlen;
        }

        reslen = slen + count * (newlen - oldlen);
        res = malloc(reslen + 1);
        if (!res)
                return NULL;

        p = s;
        rp = res;
        while ((found = strstr(p, old))) {
                size_t prelen = found - p;
                
                memcpy(rp, p, prelen);
                rp += prelen;

                memcpy(rp, new, newlen);
                rp += newlen;
                
                p = found + oldlen;
        }

        memcpy(rp, p, strlen(p) + 1);

        return res;
}
