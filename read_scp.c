#include <string.h>
#include "easyecg.h"

/*
routines cherry-picked and adapted from 8 March 2014 revision of parsescp.c
Copyright (C) 2000-2014 George B. Moody and Edna S. Moody

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/

static unsigned short get16(const char *p)
{
    unsigned short result;

    result  = *p++ & 0xff;
    result |= *p << 8;
    return (result);
}

static unsigned long get32(const char *p)
{
    unsigned long result;

    result  =  *p++ & 0xff;
    result |= (*p++ & 0xff) << 8;
    result |= (*p++ & 0xff) << 16;
    result |=  *p << 24;
    return (result);
}

static unsigned short getcrc(const char *p, long length)
{
    unsigned char a, b, crchigh = 0xff, crclow = 0xff;
    unsigned short crc;

    while (length-- > 0L) {
	crchigh = a = *p++ ^ crchigh;
	a >>= 4;		/* top 4 bits of a are now zero */
	a ^= crchigh;		

	/* swap crchigh, crclow */ 
	crchigh = crclow;
	crclow = a;

	/* rotate a left by four bits (swap nibbles) */
	b = (a & 0xf) << 4;	
 	a >>= 4;
	b = a |= b;
	
	/* rotate a left by one more bit */
	if (a & 0x80) a = (a << 1) | 1;
	else a <<= 1;

	crchigh ^= a &= 0x1f;
	a = b & 0xf0;
	crchigh ^= a;		/* crchigh complete */

	/* rotate b left by one bit */
	if (b & 0x80) b = (b << 1) | 1;
	else b <<= 1;

	crclow ^= b &= 0xe0;	/* crclow complete */
    }
    crc = (crchigh << 8) | crclow;
    return (crc);
}

static int crcok(const char *p, long length, unsigned short crcref)
{
    unsigned short crc;

    crc = getcrc(p, length);

    return (crc == crcref);
}

static int section1(const unsigned char *p, long len, struct easyecg_context *ctx)
{
	unsigned short vlen;

	p += 16; len -= 16; /* move to data area */
	if (len < 3)
		return (0);
	while (len > 2)
	{
		if (*p == 255) break;	/* tag 255 is the terminator */
		vlen = get16(p+1);
		if (vlen > len)
			return (0);
		switch (*p)
		{
		case 25:
			ctx->date.year = get16(p+3);
			ctx->date.month = *(p+5);
			ctx->date.day = *(p+6);
			break;
		case 26:
			ctx->time.hour = *(p+3);
			ctx->time.minute = *(p+4);
			ctx->time.second = *(p+5);
			break;
		}

		p += vlen+3; len -= vlen+3;
	}
}

static int section6(const unsigned char *p, long len, struct easyecg_context *ctx)
{
	p += 16; len -= 16; /* move to data area */

	if (len < 8)
		return (0);

	ctx->amplitude = get16(p);
	ctx->interval = get16(p+2);
	ctx->datalen = get16(p+6);
	ctx->dataptr = p+8;
}

int read_scp(const unsigned char *data, unsigned long file_size, struct easyecg_context *ctx)
{
	unsigned short crc, sec_id;
	unsigned long length, sec_len;
	const unsigned char *p;

	memset(ctx, 0, sizeof(ctx));

	crc = get16(data);
	length = get32(data+2);

	if (!crcok(data+2, length-2, crc))
		return -1;

	p = data + 6;	/* skip the record header -- p points to section 0 */

	while (p < data + length)	/* true if there is at least 1 more section */
	{
		crc = get16(p);
		sec_id = get16(p+2);
		sec_len = get32(p+4);
		if (sec_len < 8)
			return -2;
		if (sec_len > data + length - p)
		    return -3;

		if (!crcok(p+2, sec_len-2, crc))
			return -4;

		switch (sec_id)
		{
		case 1:
			section1(p, sec_len, ctx);
			break;
		case 6:
			section6(p, sec_len, ctx);
			break;
		}

		p += sec_len;
	}

	return 0;
}

