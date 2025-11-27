/* Lightweight File Operation Library */

/* The libfile library provides a set of lightweight utility functions for
 * common file and directory operations. 
 *
 *                           Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
 *                                               <https://github.com/qianjunfan>
 *
 *                             Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
 *
 *                                            (Nov 23, 2025) Version Alpha 0.1.1
 */

#ifndef LIBFILE_H
#define LIBFILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* File Reading Operations */

/* Get the number of lines in a file. 
 * Returns 0 on error or if file is empty/non-existent. 
 */
uint64_t file_le(const char *file_path);

/* Read all lines of a file into a NULL-terminated string array.
 * Each string is dynamically allocated and freed by the user.
 * The newline character is trimmed. Returns NULL on error.
 */
char **file_ls(const char *file_path);

/* Read the line content at a specific index (1-based) in a file.
 * Returns a dynamically allocated string with the newline trimmed, 
 * or NULL on error/out of bounds.
 */
char *file_li(const char *file_path, uint64_t cnt);

/* Check if the given path exists and is a regular file. */
bool file_ex(const char *file_path);


/* File Writing Operations */

/* Writes a string to a file, overwriting existing content.
 * Returns true on success, false on error.
 */
bool file_ws(const char *file_path, const char *content);

/* Appends a string to the end of a file.
 * Returns true on success, false on error.
 */
bool file_wa(const char *file_path, const char *content);

/* Writes a NULL-terminated string array (lines) to a file, overwriting 
 * existing content. Each string in the array will be written to a new line.
 * Returns true on success, false on error.
 */
bool file_wls(const char *file_path, char **lines);


/* Directory Operations */

/* Check if the given path exists and is a directory. */
bool dir_ex(const char *dict_path);

/* Get the number of entries (including '.' and '..') in a directory. 
 * Returns 0 on error. 
 */
uint64_t dir_le(const char *dict_path);

/* Read all directory entry names (including '.' and '..') into a 
 * NULL-terminated string array.
 * Each string is dynamically allocated and freed by the user.
 * Returns NULL on error.
 */
char **dir_ls(const char *dict_path);

#endif