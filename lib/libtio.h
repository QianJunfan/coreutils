/* Lightweight Terminal Input/Output Library */

/*
 * libtio is a lightweight terminal input/output library 
 * (currently only includes input functions)
 * It can implement single-line input or multi-line input.
 * However, the multi-line input function, io_readm, still has numerous bugs.
 *
 * To be honest, fixing these bugs is driving me crazy. 
 * Why is the terminal so complicated?
 * All in all, it is not yet complete enough for immediate production use.
 *
 *                           Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
 *                                               <https://github.com/qianjunfan>
 *
 *                             Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
 *                                            (Nov 27, 2025) Version Alpha 0.0.1
 */

#ifndef LIBIO_H
#define LIBIO_H

#include <stdio.h>
#include <stdlib.h>

char *io_readl(void);
char **io_readm(int *cnt, char terminator);

#endif /* LIBIO_H */
