#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp.h"
#include "scale.h"

char default_filename[] = "upscaled.bmp";

int main(int argc, char const *argv[])
{
    
    BMPImage *src = read_bmp(argv[1]); 
    BMPImage *target = read_bmp(argv[2]); 
    compare_bmp(src, target);

    return 0;
}