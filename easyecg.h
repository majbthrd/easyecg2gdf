#ifndef __EASY_ECG_H
#define __EASY_ECG_H

struct easyecg_context
{
	unsigned short amplitude, interval;
	const unsigned char *dataptr;
	unsigned short datalen;
	struct
	{
		unsigned short year;
		unsigned char month, day;
	} date;
	struct
	{
		unsigned char hour, minute, second;
	} time;
};

#endif

