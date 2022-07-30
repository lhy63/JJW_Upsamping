#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp.h"
#include "scale.h"

char default_filename[] = "upscaled.bmp";

void usage()
{
    printf("Usage: ./upscale <filename> <ouput_width> <output_height> <threshold> [<output_filename>]\n");
}

int main(int argc, char const *argv[])
{
    if (argc < 4) {
        usage();
        exit(1);
    }

    
    BMPImage *img = read_bmp(argv[1]); 
    BMPImage *res = bmp_upsampling_fastNedi(img, atoi(argv[2]), atoi(argv[3]), argc > 4 ? atoi(argv[4]) : 144);

    return write_bmp(res, argc > 5 ? argv[5] : default_filename);
}
