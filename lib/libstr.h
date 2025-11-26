/* Lightweight C String Manipulation Library */

/*
 * This library provides efficient and common string utilities missing 
 * from the standard C library (string.h). 
 *
 *                           Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
 *                                               <https://github.com/qianjunfan>
 *
 *                             Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
 *                                            (Nov 26, 2025) Version Alpha 0.1.0
 */


#ifndef LIBSTR_H
#define LIBSTR_H

#include <stddef.h> /* For size_t */

/* Memory management function */
void str_arr_free(char **res, int count);

/* Splitting and Joining */
char **strsplit(const char *s, char delim, int *cnt);
char *strjoin(char **arr, int count, const char *delim);

/* Trimming (in-place modification) */
char *strltrim(char *s);
char *strrtrim(char *s);
char *strtrim(char *s);

/* Case Conversion (in-place modification) */
char *strtolower(char *s);
char *strtoupper(char *s);

/* Replacement (returns new allocated string) */
char *strreplace(const char *s, const char *old, const char *new);

/* Comparison and Checking */
int streqi(const char *s1, const char *s2);
int strstarts(const char *s, const char *start);
int strends(const char *s, const char *end);

#endif
