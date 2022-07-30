#!/bin/sh

fpga_out_path=~/fpga_out/
bmps=$(ls $fpga_out_path)

for bmpname in $bmps; do
	echo "cmp $bmpname between cmodel and fpga !";
	make cmp SRC_BMP=$bmpname TARGET_BMP=$bmpname -s;
	echo "finish cmp $cmpbmpname !";
done