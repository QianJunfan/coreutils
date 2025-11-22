/* Lightweight Command-line Argument Parsing Library */

/*
 * This library is currently being used by Coreutils for command-line argument 
 * parsing.
 * It currently lacks many features and contains numerous bugs.
 * Meanwhile, features are being implemented, and bugs are being resolved.
 *
 *
 *                           Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
 *                                               <https://github.com/qianjunfan>
 *
 *                             Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
 * 
 *                                                           Version Alpha 0.1.0
 */

#ifndef LIBARG_H
#define LIBARG_H

#include <string.h>


void argd(void);
void argc(char *name);
void argo(char *cmd, char *sname, char lname);
int  argi(int argc, char **argv, void (*reg)(void), void (*guide)(void));
bool arge(char *cmd, char *opt);

#endif /* LIBARG_H */
