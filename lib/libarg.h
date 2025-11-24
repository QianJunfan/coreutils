/* Lightweight Command-line Argument Parsing Library */

/*
 * This library is currently being used by Coreutils for command-line argument 
 * parsing.
 * Many bugs and memory issues have been fixed in this update, making libarg
 * truly ready to use.
 * The next update will focus on documentation and the implementation of
 * positional arguments.
 * 
 *                           Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
 *                                               <https://github.com/qianjunfan>
 *
 *                             Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
 * 
 *                                            (Nov 23, 2025) Version Alpha 0.1.1
 */

#ifndef LIBARG_H
#define LIBARG_H
#include <stdbool.h>
#include <string.h>

/* Free all memory allocated by the libarg runtime system. */
void argd(void);

/* Register a new command (e.g., 'clone' in 'git clone'). */
void argc(char *name);

/* Register an option under a command. */
void argo(char *cmd, char *sname, char *lname);

/*
 * Initialize runtime, execute registration function (reg),
 * and parse the arguments (argv). Returns 0 on success.
 */
int  argi(char *program_name, int argc, char **argv, 
		void (*reg)(void), void (*guide)(void));

/* Check if a command (cmd) or an option (opt) was used. */
bool arge(char *cmd, char *opt);

#endif /* LIBARG_H */
