#ifndef __SCALE_H__
#define __SCALE_H__

#include "bmp.h"

BMPImage *bmp_downsampling(BMPImage *img, uint32_t width_out, uint32_t height_out);
BMPImage *bmp_upsampling_fastNedi(BMPImage *img, uint32_t width_out, uint32_t height_out, uint32_t threshold);

#endif /* __SCALE_H__ */