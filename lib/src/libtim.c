#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#include "../libtim.h"

struct runtime {
	union {
		uint16_t data[8];
		
		struct {
			uint16_t year;
			uint16_t month;
			uint16_t day;
			uint16_t hour;
			uint16_t minute;
			uint16_t second;
			uint16_t season;
			uint16_t weekday;
		};
	};
};

static struct runtime rt;

static void init(void)
{
	memset(&rt, 0, sizeof(rt));
}

static int futc(void)
{
	time_t rawtime;
	struct tm *info;
	int gmtoff = 0;

	time(&rawtime);

	info = localtime(&rawtime);

	if (info == NULL) {
		fprintf(stderr, "libtim: Failed to get local time info.\n");
		return 0;
	}

	gmtoff = info->tm_gmtoff;
	
	return gmtoff;
}


static void sync(int utc_offset_sec)
{
	time_t rawtime;
	struct tm *info;
	time_t target_time;

	time(&rawtime);

	target_time = rawtime + utc_offset_sec;

	info = gmtime(&target_time);

	if (info == NULL) {
		fprintf(stderr, "libtim: Failed to get GMT time.\n");
		return;
	}

	rt.year    = (uint16_t) (info->tm_year + 1900);
	rt.month   = (uint16_t) (info->tm_mon + 1);
	rt.day     = (uint16_t) info->tm_mday;
	rt.hour    = (uint16_t) info->tm_hour;
	rt.minute  = (uint16_t) info->tm_min;
	rt.second  = (uint16_t) info->tm_sec;
	rt.weekday = (uint16_t) info->tm_wday;
	
	rt.season  = 0;
}

uint16_t *timf(void)
{
	int utc_offset_sec = futc();
	
	init();
	sync(utc_offset_sec);
	
	return rt.data;
}

uint16_t *timuf(int utc_offset)
{
	int utc_offset_sec = utc_offset * 3600;
	
	init();
	sync(utc_offset_sec);
	
	return rt.data;
}


uint16_t *timsw(int utc_offset, uint16_t time_data[8])
{
	struct tm  tmp_tm;
	time_t     local_time;
	struct tm  *utc_info;

	memset(&tmp_tm, 0, sizeof(struct tm));
	
	tmp_tm.tm_year = time_data[0] - 1900;
	tmp_tm.tm_mon  = time_data[1] - 1;
	tmp_tm.tm_mday = time_data[2];
	tmp_tm.tm_hour = time_data[3];
	tmp_tm.tm_min  = time_data[4];
	tmp_tm.tm_sec  = time_data[5];
	tmp_tm.tm_wday = time_data[7];
	tmp_tm.tm_isdst = -1;

	tmp_tm.tm_hour -= utc_offset;

	local_time = mktime(&tmp_tm);

	utc_info = gmtime(&local_time);

	if (utc_info == NULL) {
		init();
		return rt.data;
	}

	init();
	
	rt.year    = (uint16_t) (utc_info->tm_year + 1900);
	rt.month   = (uint16_t) (utc_info->tm_mon + 1);
	rt.day     = (uint16_t) utc_info->tm_mday;
	rt.hour    = (uint16_t) utc_info->tm_hour;
	rt.minute  = (uint16_t) utc_info->tm_min;
	rt.second  = (uint16_t) utc_info->tm_sec;
	rt.weekday = (uint16_t) utc_info->tm_wday; 
	rt.season  = 0;

	return rt.data;
}
