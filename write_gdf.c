/*
rudimentary routines to write a GDF header
Copyright (C) 2017 Peter Lawrence

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

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "write_gdf.h"

/*
This code is a means to an end to write a header for a single-channel GDF file
without depending on the user having to install and maintain external libraries and 
without the need to invoke the usage of object-oriented source code
*/

int write_gdf_header(FILE *output, struct write_gdf_parameters *parms)
{
	const unsigned long channels = 1;
	unsigned char *gdf;
	unsigned long header_size = (channels + 1) * 256;
	unsigned long pos;
	union
	{
		unsigned short uint16;
		unsigned long uint32;
		double float64;
		float float32;
	} val;

	if (!output)
		return -1;

	gdf = (unsigned char *)malloc(header_size);

	if (!gdf)
		return -1;

	memset(gdf, 0, header_size);

	/*
	start of Fixed Header
	*/

	memcpy(gdf + 0, "GDF 2.10", 8);

	val.uint16 = channels + 1; memcpy(gdf + 184, &val.uint16, 2); /* Header length */

	val.uint32 = parms->data_points; memcpy(gdf + 236, &val.uint32, 4); /* number of data records */

	val.uint32 = 1; memcpy(gdf + 244, &val.uint32, 4); /* Duration of a data record (numerator) */

	val.uint32 = parms->sample_rate_hz; memcpy(gdf + 248, &val.uint32, 4); /* Duration of a data record (denominator) */

	val.uint16 = channels; memcpy(gdf + 252, &val.uint16, 2); /* NS: number of signals (channels) */

	/*
	start of Variable Header
	*/

	pos = 256;

	memcpy(gdf + pos, "ECG", 3);

	pos += 16; /* Label */
	pos += 80; /* type */

	memcpy(gdf + pos, "mV", 2); /* older format units */
	pos += 6;
	val.uint16 = 4256 + 18 /* newer format units: Milli Volts */; memcpy(gdf + pos, &val.uint16, 2); /* Physical dimension code */
	pos += 2;

	val.float64 = parms->min; memcpy(gdf + pos, &val.float64, 8);
	pos += 8;
	val.float64 = parms->max; memcpy(gdf + pos, &val.float64, 8);
	pos += 8;
	val.float64 = parms->min; memcpy(gdf + pos, &val.float64, 8);
	pos += 8;
	val.float64 = parms->max; memcpy(gdf + pos, &val.float64, 8);
	pos += 8;

	pos += 76; /* filter characterization stuff */

	val.float32 = -1.0; memcpy(gdf + pos, &val.float32, 4); /* spec says: "Notch Off is indicated by a number smaller than zero (typically -1)" */
	pos += 4;

	val.uint32 = 1; memcpy(gdf + pos, &val.uint32, 4); /* Samples Per Record */
	pos += 4;

	val.uint32 = parms->data_type; memcpy(gdf + pos, &val.uint32, 4); /* Type of data */
	pos += 4;

	fwrite(gdf, 1, header_size, output);

	free(gdf);

	return 0;
}

