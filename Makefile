EASYECG2GDF_C = easyecg2gdf.c read_scp.c write_gdf.c

easyecg2gdf: $(EASYECG2GDF_C)
	gcc $(EASYECG2GDF_C) -o $@

clean:
	rm -f easyecg2gdf easyecg2gdf.exe

