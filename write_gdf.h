#ifndef __WRITE_GDF_H
#define __WRITE_GDF_H

enum gdf_data_type
{
	GDFTYP_int8 = 1,
	GDFTYP_uint8 = 2,
	GDFTYP_int16 = 3,
	GDFTYP_uint16 = 4,
	GDFTYP_int32 = 5,
	GDFTYP_uint32 = 6,
	GDFTYP_int64 = 7,
	GDFTYP_uint64 = 8,
	GDFTYP_float32 = 16,
	GDFTYP_float64 = 17,
	GDFTYP_float128 = 18,
	GDFTYP_int24 = 279,
	GDFTYP_uint24 = 535,
};

struct write_gdf_parameters
{
	unsigned long data_points, sample_rate_hz;
	enum gdf_data_type data_type;
	double min, max;
};

int write_gdf_header(FILE *output, struct write_gdf_parameters *parms);

#endif

