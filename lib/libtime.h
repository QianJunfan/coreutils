/* Lightweight Time and Date Utility Library */

/*
 * This library provides minimal functions for retrieving and manipulating 
 * current time and UTC offsets.
 * 
 * Features will only be added if there is a demand from other coreutils. 
 * Otherwise, only bug fixes will be released.
 *
 *                           Maintainer 2025 Qian Junfan <qianjunfan0@gmail.com>
 *                                               <https://github.com/qianjunfan>
 *
 *                             Copyright (C) Qian Junfan <qianjunfan0@gmail.com>
 *                  
 *                                            (Nov 25, 2025) Version Alpha 0.1.0
 */

#ifndef LIBTIME_H
#define LIBTIME_H

#include <stdint.h>

/* Get the current time with the system's UTC offset (in hours). */
uint16_t *timuf(int utc_offset);

/* Get the current time using the system's local time (auto UTC offset). */
uint16_t *timf(void);

/* * Convert a given time array (time_data) from a local offset (utc_offset)
 * to UTC time.
 */
uint16_t *timsw(int utc_offset, uint16_t data[8]);

#endif
