#!/bin/sh

path=/downscaled/
bmps=$(ls $path)

for bmpname in $bmps; do
	echo "run case $bmpname !";
	make run SRC_BMP=$bmpname -s;
	echo "finish run case !";
done
