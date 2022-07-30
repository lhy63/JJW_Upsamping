#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bmp.h"

BMPImage *read_bmp(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("file %s open failed!\n", filename);
        goto OPEN_FAIL;
    }

    BMPImage *img = malloc(sizeof(BMPImage));
    if (!img) {
        printf("image malloc fail!\n");
        goto IMG_MALLOC_FAIL;
    }

    fread(&img->header, 1, sizeof(BMPHeader), fp);
    if (img->header.type != 0x4D42) {
        printf("illegal bmp file %s", filename);
        goto NOT_BMP;
    }

    if (!img->header.image_size_bytes) {
        img->header.image_size_bytes = img->header.size - img->header.offset;
    }

    Pixel *pixel_data = malloc(img->header.image_size_bytes);
    if (!pixel_data) {
        printf("pixel data malloc fail!\n");
        goto PIX_MALLOC_FAIL;
    }

    rewind(fp);
    fread(pixel_data, 1, img->header.offset, fp);
    fread(pixel_data, 1,  img->header.image_size_bytes, fp);
    img->data = pixel_data;



    fclose(fp);

    return img;

PIX_MALLOC_FAIL:

NOT_BMP:
    free(img);
IMG_MALLOC_FAIL:
    fclose(fp);
OPEN_FAIL:
    return 0;
}


BMPImage *create_bmp(uint32_t width, uint32_t height) {
    BMPImage *img = malloc(sizeof(BMPImage));
    if (!img) {
        printf("img create malloc fail!\n");
        return 0;
    }
    memset(img, 0, sizeof(BMPImage));
    img->data = malloc(width * height * 3);
    if (!img->data) {
        printf("img create data malloc fail!\n");
        return 0;
    }

    BMPHeader *h = &img->header;
    h->type = 0x4D42;
    h->size = width * height * 3 + 54;
    h->offset = 54;
    h->dib_header_size = 40;
    h->width_px = width;
    h->height_px = height;
    h->num_planes = 1;
    h->bits_per_pixel = 24;
    h->compression = 0;
    h->image_size_bytes = width * height * 3;


    return img;
}


int32_t   write_bmp(BMPImage *img, const char *filename)
{
    FILE *fp = fopen(filename, "wb+");
    if (!fp) {
        printf("open/create file %s failed!\n", filename);
        return -1;
    }

    fwrite(&img->header, 1, sizeof(BMPHeader), fp);
    fwrite(img->data, 1, img->header.image_size_bytes, fp);
    return 0;
}

Pixel    *bmp_pixel_at(BMPImage *img, int x, int y, uint32_t mode )
{
    if (x > img->header.width_px - 1) {
        if(mode == 0){
            x = img->header.width_px - 1;
        } else if (mode == 1)
        {
            x = img->header.width_px - 4;
        } else if (mode == 2) {
            x = img->header.width_px - 2;
        }  
    } else if (x < 0) {
        if(mode == 2){
            x = 1;
        } else {
            x = 0;
        }
    }
    if(y > img->header.height_px - 1) {
        if(mode == 0) {
            y = img->header.height_px - 1;
        } else if (mode == 1) {
             y = img->header.height_px - 4;
        }else if (mode == 2) {
            y = img->header.height_px - 2;
        }
    }else if (y < 0) {
        if(mode == 2){
            y= 1;
        } else {
            y = 0;
        }
    }

    if(img->header.height_px >= 0){
      return (Pixel *)((void *)img->data + (x + (img->header.height_px - y - 1) * img->header.width_px) * img->header.bits_per_pixel / 8);
    }else{
      return (Pixel *)((void *)img->data + (x + y * img->header.width_px) * img->header.bits_per_pixel / 8);
    }

}

void print_bmp_header(BMPImage *img)
{
    BMPHeader *h = &img->header;
    Pixel *d = img->data;
    printf("type=%x\n", h->type);             
    printf("size=%u\n", h->size);            
    printf("reserved1=%u\n", h->reserved1);       
    printf("reserved2=%u\n", h->reserved2);       
    printf("offset=%u\n", h->offset);           
    printf("dib_header_size=%u\n", h->dib_header_size);  
    printf("width_px=%d\n", h->width_px);        
    printf("height_px=%d\n", h->height_px);       
    printf("num_planes=%u\n", h->num_planes);       
    printf("bits_per_pixel=%u\n", h->bits_per_pixel);  
    printf("compression=%u\n", h->compression);     
    printf("image_size_bytes=%u\n", h->image_size_bytes);
    printf("x_resolution_ppm=%d\n", h->x_resolution_ppm);
    printf("y_resolution_ppm=%d\n", h->y_resolution_ppm);
    printf("num_colors=%u\n", h->num_colors);      
    printf("important_colors=%u\n", h->important_colors);
    for(int i = 0; i < h->image_size_bytes / 3; i++){
        uint8_t *pb = &(d->b) + i * 4;
        uint8_t *pg = &(d->g) + i * 4;
        uint8_t *pr = &(d->r) + i * 4;
        printf("the r pixels is : %d\n", *pr);
        printf("the g pixels is : %d\n", *pg);
        printf("the b pixels is : %d\n", *pb);
    }
}


void compare_bmp(BMPImage *src,  BMPImage *target)
{
       int ret = 0; 
       for (uint32_t y = 0; y < src->header.height_px; y++) {
          for (uint32_t x = 0; x < src->header.width_px; x++) {
              Pixel *srcP = bmp_pixel_at(src, x, y, 0);
              Pixel *targetP = bmp_pixel_at(target, x, y, 0);
              if(srcP->b != targetP->b){
                  printf("b channel not equal at x : %d    y : %d \n", x, y);
                  ret = 1;
              }

              if(srcP->g != targetP->g){
                  printf("g channel not equal at x : %d    y : %d\n", x, y);
                  ret = 1;
              }

              if(srcP->r != targetP->r){
                  printf("r channel not equal at x : %d    y : %d\n", x, y);
                  ret = 1;
              }
        }
    }

    if(ret != 0){
        printf("two bmp file are not equal ! \n");
    }else{
        printf("two bmp file are equal ! \n");
    }
}