#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp.h"

BMPImage *bmp_downsampling(BMPImage *img, uint32_t width_out, uint32_t height_out)
{
    if (img->header.width_px < width_out || img->header.height_px < height_out) {
        printf("downsampling should have smaller dimensions!\n");
        return 0;
    }

    if (!width_out || !height_out) {
        printf("?\n");
        return 0;
    }

    uint32_t w = img->header.width_px;
    uint32_t h = img->header.height_px;
    uint32_t w_o = width_out;
    uint32_t h_o = height_out;

    BMPImage *res = create_bmp(width_out, height_out);

    uint32_t num_sample_x = w / w_o; 
    uint32_t num_sample_y = h / h_o; 

    uint32_t x, y, _x, _y;
    for (y = 0; y < height_out; y++) {
        for (x = 0; x < width_out; x++) {
            uint32_t _x_base = x * w / w_o;
            uint32_t _y_base = y * h / h_o;
            uint32_t num_accum = 0;
            uint32_t ac_r = 0, ac_g = 0, ac_b = 0; //accumulated color channels
            for (_y = 0; _y < num_sample_y && _y + _y_base < h; _y++) {
                for (_x = 0; _x < num_sample_x && _x + _x_base < w; _x++) {
                    Pixel *p = bmp_pixel_at(img, _x + _x_base, _y + _y_base, 0);
                    ac_r += p->r;
                    ac_g += p->g;
                    ac_b += p->b;
                    num_accum++;
                }
            }
            Pixel *t = bmp_pixel_at(res, x, y, 0);
            t->r = (uint8_t)(ac_r / num_accum);
            t->g = (uint8_t)(ac_g / num_accum);
            t->b = (uint8_t)(ac_b / num_accum);
        }
    }
    return res;
}


uint32_t edgeArray[3][2160][3840] = {} ; 

uint32_t isEdge(int x, int y, uint32_t c, uint32_t width_out, uint32_t height_out) {
    uint32_t ret = 0 ;

    if(x  >  width_out - 1){ x = width_out - 2;}
    if(x < 0){x = 1;}
    if(y > height_out - 1){ y = height_out - 2;}
    if(y < 0) {y = 1;}
    ret = edgeArray[c][y][x];
    return ret;
}

BMPImage *bmp_upsampling_fastNedi(BMPImage *img, uint32_t width_out, uint32_t height_out, uint32_t threshold) {
    if (img->header.width_px > width_out || img->header.height_px > height_out) {
        printf("upsampling should have bigger dimensions!\n");
        return 0;
    }

    if (!width_out || !height_out) {
        printf("?\n");
        return 0;
    }

    
    BMPImage *res = create_bmp(width_out, height_out);
    
    uint32_t w = img->header.width_px;
    uint32_t h = img->header.height_px;  
    
    uint32_t yScale = height_out / h;
    uint32_t xScale = width_out / w;
    if(yScale != xScale){
      printf("the scale size is invalid !\n");
      return 0;
    }

    for(uint32_t i = 0; i < 2160; i ++){
        for(uint16_t j = 0; j < 3840; j ++){
            edgeArray[0][i][j] = 0;
            edgeArray[1][i][j] = 0;
            edgeArray[2][i][j] = 0;
        }
    }


    for (uint32_t y = 0; y < height_out; y++) {
        for (uint32_t x = 0; x < width_out; x++) {
               if(x % 4 == 0 && y % 4 == 0){
                     Pixel * target =  bmp_pixel_at(res, x, y, 0);
                     Pixel * src = bmp_pixel_at(img, x / 4, y / 4, 0);
                     target->b = src->b;
                     target->g = src->g;
                     target->r  = src->r;
                     edgeArray[0][y][x] = 0;
                     edgeArray[1][y][x] = 0;
                     edgeArray[2][y][x] = 0;
                }

                if(x % 4 == 2 && y % 4 == 0){
                     Pixel * target =  bmp_pixel_at(res, x, y, 0);
                     Pixel * srcLeft = bmp_pixel_at(img, (x - 2) / 4, y / 4, 0);
                     Pixel * srcRight = bmp_pixel_at(img, (x + 2) / 4, y / 4, 0);
                    if(abs(srcLeft->b - srcRight->b) >= threshold){
                        target->b = srcLeft->b;
                    }else{
                        target->b = (srcLeft->b  + srcRight->b) / 2 ;
                    }

                    if(abs(srcLeft->g - srcRight->g) >= threshold){
                        target->g = srcLeft->g;
                    }else{
                        target->g = (srcLeft->g + srcRight->g) / 2 ;
                    }

                     if(abs(srcLeft->r - srcRight->r) >= threshold){
                        target->r = srcLeft->r;
                    }else{
                        target->r = (srcLeft->r  + srcRight->r )/ 2 ;
                    }

                     edgeArray[0][y][x] = 0;
                     edgeArray[1][y][x] = 0;
                     edgeArray[2][y][x] = 0;
                }

                if(x % 4 == 0 && y % 4 == 2){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * srcUp = bmp_pixel_at(img, x / 4, (y - 2) / 4, 0);
                    Pixel * srcDown = bmp_pixel_at(img, x / 4, (y + 2) / 4, 0);
                    if(abs(srcUp->b - srcDown->b) >= threshold){
                        target->b = srcUp->b;
                    }else{
                        target->b = (srcUp->b + srcDown->b) / 2 ;
                    }

                    if(abs(srcUp->g - srcDown->g) >= threshold){
                        target->g = srcUp->g;
                    }else{
                        target->g = (srcUp->g + srcDown->g) / 2 ;
                    }

                    if(abs(srcUp->r - srcDown->r) >= threshold){
                        target->r = srcUp->r;
                    }else{
                        target->r = (srcUp->r + srcDown->r) / 2 ;
                    }

                     edgeArray[0][y][x] = 0;
                     edgeArray[1][y][x] = 0;
                     edgeArray[2][y][x] = 0;

                }

                if(x % 4 == 2 && y % 4 == 2){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * srcLeftUp = bmp_pixel_at(img, (x - 2) / 4, (y - 2) / 4, 0);
                    Pixel * srcRightDown = bmp_pixel_at(img, (x + 2) / 4, (y + 2) / 4, 0);
                    Pixel * srcLeftDown = bmp_pixel_at(img, (x - 2) / 4, (y + 2) / 4, 0);
                    Pixel * srcRightUp = bmp_pixel_at(img, (x + 2) / 4, (y - 2) / 4, 0);

                    uint32_t mainDiffInB = abs(srcLeftUp->b - srcRightDown->b);
                    uint32_t mainDiffInG = abs(srcLeftUp->g - srcRightDown->g);
                    uint32_t mainDiffInR = abs(srcLeftUp->r - srcRightDown->r);

                    uint32_t counterDiffInB = abs(srcLeftDown->b - srcRightUp->b);
                    uint32_t counterDiffInG = abs(srcLeftDown->g - srcRightUp->g);
                    uint32_t counterDiffInR = abs(srcLeftDown->r - srcRightUp->r);

                    uint32_t minDiffInB = 0;
                    uint32_t minDiffInG = 0;
                    uint32_t minDiffInR = 0;
                    if(mainDiffInB >= counterDiffInB){minDiffInB = counterDiffInB;}
                    else{minDiffInB = mainDiffInB;}

                    if(mainDiffInG >= counterDiffInG){minDiffInG = counterDiffInG;}
                    else{minDiffInG = mainDiffInG;}

                    if(mainDiffInR >= counterDiffInR){minDiffInR = counterDiffInR;}
                    else{minDiffInR = mainDiffInR;}
                    
                    if(minDiffInB >= threshold){
                        if(mainDiffInB >= counterDiffInB){
                            target->b = srcLeftUp->b;
                        }else{
                            target->b = srcLeftDown->b;
                        }
                    }else{
                        if(mainDiffInB >= counterDiffInB){
                            target ->b = (srcLeftDown->b+ srcRightUp->b )/ 2 ;
                        }else{
                            target ->b = (srcLeftUp->b + srcRightDown->b) / 2 ;
                        }
                    }

                    if(minDiffInG >= threshold){
                        if(mainDiffInG >= counterDiffInG){
                            target->g = srcLeftUp->g;
                        }else{
                            target->g = srcLeftDown->g;
                        }
                    }else{
                        if(mainDiffInG >= counterDiffInG){
                            target ->g = (srcLeftDown->g + srcRightUp->g )/ 2 ;
                        }else{
                            target ->g = (srcLeftUp->g + srcRightDown->g) / 2 ;
                        }
                    }

                    if(minDiffInR >= threshold){
                        if(mainDiffInR >= counterDiffInR){
                            target->r = srcLeftUp->r;
                        }else{
                            target->r = srcLeftDown->r;
                        }
                    }else{
                        if(mainDiffInR >= counterDiffInR){
                            target ->r = (srcLeftDown->r + srcRightUp->r) / 2 ;
                        }else{
                            target ->r = (srcLeftUp->r + srcRightDown->r) / 2 ;
                        }
                    }

                    edgeArray[0][y][x] = 0;
                    edgeArray[1][y][x] = 0;
                    edgeArray[2][y][x] = 0;
                 
                }        

        }
    }

     for (int y = 0; y < height_out; y++) {
        for (int x = 0; x < width_out; x++) {

            if(x % 4 == 1 && y % 4 == 0){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeft = bmp_pixel_at(res, x - 1, y , 1);
                    target ->b = targetLeft ->b;
                    target ->g = targetLeft ->g;
                    target ->r  = targetLeft ->r;

                    edgeArray[0][y][x] = 0;
                    edgeArray[1][y][x] = 0;
                    edgeArray[2][y][x] = 0;
                }

                if(x % 4 == 3 && y % 4 == 0){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeft = bmp_pixel_at(res, x - 3, y , 1);
                    Pixel * targetRight = bmp_pixel_at(res, x + 1, y , 1);

                    if(abs(targetLeft->b - targetRight->b) >= threshold){
                        target ->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        target->b = (targetLeft->b + targetRight ->b) / 2;
                        edgeArray[0][y][x] = 0;
                    }

                    if(abs(targetLeft->g - targetRight->g) >= threshold){
                        target ->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        target->g = (targetLeft->g + targetRight ->g) / 2;
                        edgeArray[1][y][x] = 0;
                    }

                    if(abs(targetLeft->r - targetRight->r) >= threshold){
                        target ->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        target->r = (targetLeft->r + targetRight ->r) / 2;
                        edgeArray[2][y][x] = 0;
                    }

                }

                if(x % 4 == 0 && y % 4 == 1) {
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetUp = bmp_pixel_at(res, x, y - 1 , 1);
                    target -> b = targetUp -> b;
                    target -> g = targetUp -> g;
                    target -> r  = targetUp -> r;

                    edgeArray[0][y][x] = 0;
                    edgeArray[1][y][x] = 0;
                    edgeArray[2][y][x] = 0;
                }

                 if(x % 4 == 1 && y % 4 == 1) {
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 1, y - 1 , 1);
                    target -> b = targetLeftUp -> b;
                    target -> g = targetLeftUp -> g;
                    target -> r  = targetLeftUp -> r;

                    edgeArray[0][y][x] = 0;
                    edgeArray[1][y][x] = 0;
                    edgeArray[2][y][x] = 0;
                }

                 if(x % 4 == 2 && y % 4 == 1) {
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 2, y - 1 , 1);
                    target -> b = targetLeftUp -> b;
                    target -> g = targetLeftUp -> g;
                    target -> r  = targetLeftUp -> r;

                    edgeArray[0][y][x] = 0;
                    edgeArray[1][y][x] = 0;
                    edgeArray[2][y][x] = 0;
                }
                 

                if(x % 4 == 3 && y % 4 == 1){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 3, y - 1 , 1);
                    Pixel * targetRightUp = bmp_pixel_at(res, x + 1, y - 1 , 1);

                    if(abs(targetLeftUp->b - targetRightUp->b) >= threshold){
                        target ->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        target->b = (targetLeftUp->b + targetRightUp ->b) / 2;
                        edgeArray[0][y][x] = 0;
                    }

                    if(abs(targetLeftUp->g - targetRightUp->g) >= threshold){
                        target ->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        target->g = (targetLeftUp->g + targetRightUp ->g) / 2;
                        edgeArray[1][y][x] = 0;
                    }

                    if(abs(targetLeftUp->r - targetRightUp->r) >= threshold){
                        target ->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        target->r = (targetLeftUp->r + targetRightUp ->r) / 2;
                        edgeArray[2][y][x] = 0;
                    }        

                } 

                if(x % 4 == 1 && y % 4 == 2){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 1, y - 2 , 1);
                    target -> b = targetLeftUp -> b;
                    target -> g = targetLeftUp -> g;
                    target -> r  = targetLeftUp -> r;

                    edgeArray[0][y][x] = 0;
                    edgeArray[1][y][x] = 0;
                    edgeArray[2][y][x] = 0;
                }

               if(x % 4 == 3 && y % 4 == 2){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 3, y - 2 , 1);
                    Pixel * targetRightUp = bmp_pixel_at(res, x + 1, y - 2 , 1);

                    if(abs(targetLeftUp->b - targetRightUp->b) >= threshold){
                        target ->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        target->b = (targetLeftUp->b + targetRightUp ->b) / 2;
                        edgeArray[0][y][x] = 0;
                    }

                    if(abs(targetLeftUp->g - targetRightUp->g) >= threshold){
                        target ->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        target->g = (targetLeftUp->g + targetRightUp ->g) / 2;
                        edgeArray[1][y][x] = 0;
                    }

                    if(abs(targetLeftUp->r - targetRightUp->r) >= threshold){
                        target ->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        target->r = (targetLeftUp->r + targetRightUp ->r )/ 2;
                        edgeArray[2][y][x] = 0;
                    }

                }

                if(x % 4 == 0 && y % 4 == 3){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetUp = bmp_pixel_at(res, x, y - 3 , 1);
                    Pixel * targetDown = bmp_pixel_at(res, x, y + 1 , 1);

                    if(abs(targetUp->b - targetDown->b) >= threshold){
                        target ->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        target->b =( targetUp->b + targetDown ->b) / 2;
                        edgeArray[0][y][x] = 0;
                    }

                    if(abs(targetUp->g - targetDown->g) >= threshold){
                        target ->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        target->g =( targetUp->g + targetDown ->g) / 2;
                        edgeArray[1][y][x] = 0;
                    }

                    if(abs(targetUp->r - targetDown->r) >= threshold){
                        target ->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        target->r = (targetUp->r + targetDown ->r) / 2;
                        edgeArray[2][y][x] = 0;
                    }

                }


                  if(x % 4 == 1 && y % 4 == 3){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 1, y - 3 , 1);
                    Pixel * targetLeftDown = bmp_pixel_at(res, x - 1, y + 1 , 1);

                    if(abs(targetLeftUp->b - targetLeftDown->b) >= threshold){
                        target ->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        target->b = (targetLeftUp->b + targetLeftDown ->b) / 2;
                        edgeArray[0][y][x] = 0;
                    }

                    if(abs(targetLeftUp->g - targetLeftDown->g) >= threshold){
                        target ->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        target->g = (targetLeftUp->g + targetLeftDown ->g) / 2;
                        edgeArray[1][y][x] = 0;
                    }
                    if(abs(targetLeftUp->r - targetLeftDown->r) >= threshold){
                        target ->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        target->r = (targetLeftUp->r + targetLeftDown ->r) / 2;
                        edgeArray[2][y][x] = 0;
                    }

                }


                if(x % 4 == 2 && y % 4 == 3){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 2, y - 3 , 1);
                    Pixel * targetLeftDown = bmp_pixel_at(res, x - 2, y + 1 , 1);

                    if(abs(targetLeftUp->b - targetLeftDown->b) >= threshold){
                        target ->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        target->b = (targetLeftUp->b + targetLeftDown ->b) / 2;
                        edgeArray[0][y][x] = 0;
                    }

                    if(abs(targetLeftUp->g - targetLeftDown->g) >= threshold){
                        target ->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        target->g = (targetLeftUp->g + targetLeftDown ->g) / 2;
                        edgeArray[1][y][x] = 0;
                    }

                    if(abs(targetLeftUp->r - targetLeftDown->r) >= threshold){
                        target ->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        target->r = (targetLeftUp->r + targetLeftDown ->r) / 2;
                        edgeArray[2][y][x] = 0;
                    }

                }

                 if(x % 4 == 3 && y % 4 == 3){
                    Pixel * target =  bmp_pixel_at(res, x, y, 0);
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 3, y - 3, 1);
                    Pixel * targetRightDown = bmp_pixel_at(res, x + 1, y + 1, 1);
                    Pixel * targetLeftDown = bmp_pixel_at(res, x - 3, y + 1, 1);
                    Pixel * targetRightUp = bmp_pixel_at(res, x + 1, y - 3, 1);

                    uint32_t mainDiffInB = abs(targetLeftUp->b - targetRightDown->b);
                    uint32_t mainDiffInG = abs(targetLeftUp->g - targetRightDown->g);
                    uint32_t mainDiffInR = abs(targetLeftUp->r - targetRightDown->r);

                    uint32_t counterDiffInB = abs(targetLeftDown->b - targetRightUp->b);
                    uint32_t counterDiffInG = abs(targetLeftDown->g - targetRightUp->g);
                    uint32_t counterDiffInR = abs(targetLeftDown->r - targetRightUp->r);

                    uint32_t minDiffInB = 0;
                    uint32_t minDiffInG = 0;
                    uint32_t minDiffInR = 0;

                    if(mainDiffInB >= counterDiffInB){minDiffInB = counterDiffInB;}
                    else{minDiffInB = mainDiffInB;}

                    if(mainDiffInG >= counterDiffInG){minDiffInG = counterDiffInG;}
                    else{minDiffInG = mainDiffInG;}

                    if(mainDiffInR >= counterDiffInR){minDiffInR = counterDiffInR;}
                    else{minDiffInR = mainDiffInR;}
                    
                    if(minDiffInB >= threshold){
                        target->b = (uint8_t)(0);
                        edgeArray[0][y][x] = 1;
                    }else{
                        if(mainDiffInB >= counterDiffInB){
                            target ->b = (targetLeftDown->b + targetRightUp->b) / 2 ;
                        }else{
                            target ->b = (targetLeftUp->b + targetRightDown->b) / 2 ;
                        }
                         edgeArray[0][y][x] = 0;
                    }

                    if(minDiffInG >= threshold){
                        target->g = (uint8_t)(0);
                        edgeArray[1][y][x] = 1;
                    }else{
                        if(mainDiffInG >= counterDiffInG){
                            target ->g = (targetLeftDown->g + targetRightUp->g) / 2 ;
                        }else{
                            target ->g = (targetLeftUp->g + targetRightDown->g) / 2 ;
                        }
                        edgeArray[1][y][x] = 0;
                    }

                    if(minDiffInR >= threshold){
                        target->r = (uint8_t)(0);
                        edgeArray[2][y][x] = 1;
                    }else{
                        if(mainDiffInR >= counterDiffInR){
                            target ->r = (targetLeftDown->r + targetRightUp->r) / 2 ;
                        }else{
                            target ->r = (targetLeftUp->r + targetRightDown->r) / 2 ;
                        }
                        edgeArray[2][y][x] = 0;
                    }
                } 
        }
    }

    uint8_t diffArrayInB[4] = {};
    uint8_t candidtateArrayInB[4] = {};
    uint8_t diffValidInB[4] = {};
    uint32_t selectedInB = 0;
    for(uint32_t i = 0; i < 4 ; i ++){
        diffArrayInB[i] = 0;
        candidtateArrayInB[i] = 0;
        diffValidInB[i] = 0;
    }

    uint8_t diffArrayInG[4] = {};
    uint8_t candidtateArrayInG[4] = {};
    uint8_t diffValidInG[4] = {};
    uint32_t selectedInG = 0;
    for(uint32_t i = 0; i < 4 ; i ++){
        diffArrayInG[i] = 0;
        candidtateArrayInG[i] = 0;
        diffValidInG[i] = 0;
    }

    uint8_t diffArrayInR[4] = {};
    uint8_t candidtateArrayInR[4] = {};
    uint8_t diffValidInR[4] = {};
    uint32_t selectedInR = 0;
    for(uint32_t i = 0; i < 4 ; i ++){
        diffArrayInR[i] = 0;
        candidtateArrayInR[i] = 0;
        diffValidInR[i] = 0;
    } 
    

  for (int y = 0; y < height_out; y++) {
        for (int x = 0; x < width_out; x++) {
            Pixel * target =  bmp_pixel_at(res, x, y, 0);

            if(isEdge(x, y, 0, width_out, height_out) == 1){
                if(isEdge(x - 1, y, 0, width_out, height_out) == 0 && isEdge(x + 1, y, 0, width_out, height_out) == 0){
                    Pixel * targetLeft = bmp_pixel_at(res, x - 1, y, 2);
                    Pixel * targetRight = bmp_pixel_at(res, x + 1, y, 2);
                     diffArrayInB[0] =  abs(targetLeft->b - targetRight->b);
                     candidtateArrayInB[0] = (targetLeft->b + targetRight->b) / 2; 
                     diffValidInB[0] = (uint8_t)(1);
                }else{
                    diffArrayInB[0] =  (uint8_t)(0);
                     candidtateArrayInB[0] = (uint8_t)(0); 
                     diffValidInB[0] = (uint8_t)(0);
                }

                if(isEdge(x, y - 1, 0, width_out, height_out) == 0 && isEdge(x, y + 1, 0, width_out, height_out) == 0){
                    Pixel * targetUp = bmp_pixel_at(res, x, y - 1, 2);
                    Pixel * targetDown = bmp_pixel_at(res, x, y + 1, 2);
                     diffArrayInB[1] =  abs(targetUp->b - targetDown->b);
                     candidtateArrayInB[1] = (targetUp->b + targetDown->b) / 2; 
                     diffValidInB[1] = (uint8_t)(1);
                }else{
                    diffArrayInB[1] =  (uint8_t)(0);
                     candidtateArrayInB[1] = (uint8_t)(0); 
                     diffValidInB[1] = (uint8_t)(0);
                }

                if(isEdge(x - 1, y - 1, 0, width_out, height_out) == 0 && isEdge(x + 1, y + 1, 0, width_out, height_out) == 0){
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 1, y - 1, 2);
                    Pixel * targetRightDown = bmp_pixel_at(res, x + 1, y + 1, 2);
                     diffArrayInB[2] =  abs(targetLeftUp->b - targetRightDown->b);
                     candidtateArrayInB[2] = (targetLeftUp->b + targetRightDown->b) / 2; 
                     diffValidInB[2] = (uint8_t)(1);
                }else{
                     diffArrayInB[2] =  (uint8_t)(0);
                     candidtateArrayInB[2] = (uint8_t)(0); 
                     diffValidInB[2] = (uint8_t)(0);
                }

                if(isEdge(x - 1, y + 1, 0, width_out, height_out) == 0 && isEdge(x + 1, y - 1, 0, width_out, height_out) == 0){
                    Pixel * targetLeftDown = bmp_pixel_at(res, x - 1, y + 1, 2);
                    Pixel * targetRightUp = bmp_pixel_at(res, x + 1, y - 1, 2);
                     diffArrayInB[3] =  abs(targetLeftDown->b - targetRightUp->b);
                     candidtateArrayInB[3] = (targetLeftDown->b + targetRightUp->b) / 2; 
                     diffValidInB[3] = (uint8_t)(1);
                }else{
                     diffArrayInB[3] =  (uint8_t)(0);
                     candidtateArrayInB[3] = (uint8_t)(0); 
                     diffValidInB[3] = (uint8_t)(0);
                }

                
                if(diffValidInB[0] == 1){
                    if(diffValidInB[1] == 1){
                        if(diffValidInB[2] == 1){
                            if(diffValidInB[3] == 1){
                                if(diffArrayInB[0] <= diffArrayInB[1] && diffArrayInB[0] <= diffArrayInB[2] && diffArrayInB[0] <= diffArrayInB[3]){
                                    selectedInB = 0;
                                }else if(diffArrayInB[1] <= diffArrayInB[2] && diffArrayInB[1] <= diffArrayInB[3]){
                                    selectedInB = 1;
                                }else if(diffArrayInB[2] <= diffArrayInB[3]){
                                    selectedInB = 2;
                                }else{
                                    selectedInB = 3;
                                }
                            }else{
                                if(diffArrayInB[0] <= diffArrayInB[1] && diffArrayInB[0] <= diffArrayInB[2]){
                                    selectedInB = 0;
                                }else if(diffArrayInB[1] <= diffArrayInB[2]){
                                    selectedInB = 1;
                                }else{
                                    selectedInB = 2;
                                }
                            }
                        }else{
                            if(diffValidInB[3] == 1){
                                if(diffArrayInB[0] <= diffArrayInB[1] && diffArrayInB[0] <= diffArrayInB[3]){
                                    selectedInB = 0;
                                }else if(diffArrayInB[1] <= diffArrayInB[3]){
                                    selectedInB = 1;
                                }else{
                                    selectedInB = 3;
                                } printf("x: %d,  y %d\n", x, y);
                            }else {
                                if(diffArrayInB[0] <= diffArrayInB[1]){
                                    selectedInB = 0;
                                }else{
                                    selectedInB = 1;
                                }
                            }
                        }
                    }else{
                        if(diffValidInB[2] == 1){
                            if(diffValidInB[3]== 1){
                                if(diffArrayInB[0] <= diffArrayInB[2] && diffArrayInB[0] <= diffArrayInB[3]){
                                    selectedInB = 0;
                                }else if(diffArrayInB[2] <= diffArrayInB[3]){
                                    selectedInB = 2;
                                }else{
                                    selectedInB = 3;
                                }
                            }else{
                                if(diffArrayInB[0] <= diffArrayInB[2]){
                                    selectedInB = 0;
                                }else{
                                    selectedInB = 2;
                                }
                            }
                        }else{
                            if(diffValidInB[3] == 1){
                                if(diffArrayInB[0] <= diffArrayInB[3]){
                                    selectedInB = 0;
                                }else{
                                    selectedInB = 3;
                                }
                            }else{
                                selectedInB = 0;
                            }

                        }
                    }
                }else{
                    if(diffValidInB[1] == 1){
                        if(diffValidInB[2] == 1){
                            if(diffValidInB[3] == 1){
                                if(diffArrayInB[1] <= diffArrayInB[2] && diffArrayInB[1] <= diffArrayInB[3]){
                                    selectedInB = 1;
                                }else if(diffArrayInB[2] <= diffArrayInB[3]){
                                    selectedInB = 2;
                                }else{
                                    selectedInB = 3;
                                }
                            }else{
                                 if(diffArrayInB[1] <= diffArrayInB[2]){
                                    selectedInB = 1;
                                }else{
                                    selectedInB = 2;
                                }
                            }
                        }else{
                            if(diffValidInB[3] == 1){
                                if(diffArrayInB[1] <= diffArrayInB[3]){
                                    selectedInB = 1;
                                }else{
                                    selectedInB = 3;
                                }
                            }else{
                                selectedInB = 1;
                            }
                        }
                    }else{
                        if(diffValidInB[2] == 1){
                            if(diffValidInB[3] == 1){
                                if(diffArrayInB[2] <= diffArrayInB[3]){
                                    selectedInB = 2;
                                }else{
                                    selectedInB = 3;
                                }
                            }else{
                                selectedInB = 2;
                            }
                        }else{
                            selectedInB = 3;
                        }
                    }
                }

                target ->b = candidtateArrayInB[selectedInB];
            }

    

             if(isEdge(x, y, 1, width_out, height_out) == 1){
                if(isEdge(x - 1, y, 1, width_out, height_out) == 0 && isEdge(x + 1, y, 1, width_out, height_out) == 0){
                    Pixel * targetLeft = bmp_pixel_at(res, x - 1, y, 2);
                    Pixel * targetRight = bmp_pixel_at(res, x + 1, y, 2);
                     diffArrayInG[0] =  abs(targetLeft->g - targetRight->g);
                     candidtateArrayInG[0] = (targetLeft->g + targetRight->g) / 2; 
                     diffValidInG[0] = (uint8_t)(1);
                }else{
                    diffArrayInG[0] =  (uint8_t)(0);
                     candidtateArrayInG[0] = (uint8_t)(0); 
                     diffValidInG[0] = (uint8_t)(0);
                }

                if(isEdge(x, y - 1, 1, width_out, height_out) == 0 && isEdge(x, y + 1, 1, width_out, height_out) == 0){
                    Pixel * targetUp = bmp_pixel_at(res, x, y - 1, 2);
                    Pixel * targetDown = bmp_pixel_at(res, x, y + 1, 2);
                     diffArrayInG[1] =  abs(targetUp->g - targetDown->g);
                     candidtateArrayInG[1] = (targetUp->g + targetDown->g) / 2; 
                     diffValidInG[1] = (uint8_t)(1);
                }else{
                    diffArrayInG[1] =  (uint8_t)(0);
                     candidtateArrayInG[1] = (uint8_t)(0); 
                     diffValidInG[1] = (uint8_t)(0);
                }


                if(isEdge(x - 1, y - 1, 1, width_out, height_out) == 0 && isEdge(x + 1, y + 1, 1, width_out, height_out) == 0){
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 1, y - 1, 2);
                    Pixel * targetRightDown = bmp_pixel_at(res, x + 1, y + 1, 2);
                     diffArrayInG[2] =  abs(targetLeftUp->g - targetRightDown->g);
                     candidtateArrayInG[2] = (targetLeftUp->g + targetRightDown->g) / 2; 
                     diffValidInG[2] = (uint8_t)(1);
                }else{
                     diffArrayInG[2] =  (uint8_t)(0);
                     candidtateArrayInG[2] = (uint8_t)(0); 
                     diffValidInG[2] = (uint8_t)(0);
                }

                if(isEdge(x - 1, y + 1, 1, width_out, height_out) == 0 && isEdge(x + 1, y - 1, 1, width_out, height_out) == 0){
                    Pixel * targetLeftDown = bmp_pixel_at(res, x - 1, y + 1, 2);
                    Pixel * targetRightUp = bmp_pixel_at(res, x + 1, y - 1, 2);
                     diffArrayInG[3] =  abs(targetLeftDown->g - targetRightUp->g);
                     candidtateArrayInG[3] = (targetLeftDown->g + targetRightUp->g) / 2; 
                     diffValidInG[3] = (uint8_t)(1);
                }else{
                     diffArrayInG[3] =  (uint8_t)(0);
                     candidtateArrayInG[3] = (uint8_t)(0); 
                     diffValidInG[3] = (uint8_t)(0);
                }
               
                if(diffValidInG[0] == 1){
                    if(diffValidInG[1] == 1){
                        if(diffValidInG[2] == 1){
                            if(diffValidInG[3] == 1){
                                if(diffArrayInG[0] <= diffArrayInG[1] && diffArrayInG[0] <= diffArrayInG[2] && diffArrayInG[0] <= diffArrayInG[3]){
                                    selectedInG = 0;
                                }else if(diffArrayInG[1] <= diffArrayInG[2] && diffArrayInG[1] <= diffArrayInG[3]){
                                    selectedInG = 1;
                                }else if(diffArrayInG[2] <= diffArrayInG[3]){
                                    selectedInG = 2;
                                }else{
                                    selectedInG = 3;
                                }
                            }else{
                                if(diffArrayInG[0] <= diffArrayInG[1] && diffArrayInG[0] <= diffArrayInG[2]){
                                    selectedInG = 0;
                                }else if(diffArrayInG[1] <= diffArrayInG[2]){
                                    selectedInG = 1;
                                }else{
                                    selectedInG = 2;
                                }
                            }
                        }else{
                            if(diffValidInG[3] == 1){
                                if(diffArrayInG[0] <= diffArrayInG[1] && diffArrayInG[0] <= diffArrayInG[3]){
                                    selectedInG = 0;
                                }else if(diffArrayInG[1] <= diffArrayInG[3]){
                                    selectedInG = 1;
                                }else{
                                    selectedInG = 3;
                                }
                            }else {
                                if(diffArrayInG[0] <= diffArrayInG[1]){
                                    selectedInG = 0;
                                }else{
                                    selectedInG = 1;
                                }
                            }
                        }
                    }else{
                        if(diffValidInG[2] == 1){
                            if(diffValidInG[3]== 1){
                                if(diffArrayInG[0] <= diffArrayInG[2] && diffArrayInG[0] <= diffArrayInG[3]){
                                    selectedInG = 0;
                                }else if(diffArrayInG[2] <= diffArrayInG[3]){
                                    selectedInG = 2;
                                }else{
                                    selectedInG = 3;
                                }
                            }else{
                                if(diffArrayInG[0] <= diffArrayInG[2]){
                                    selectedInG = 0;
                                }else{
                                    selectedInG = 2;
                                }
                            }
                        }else{
                            if(diffValidInG[3] == 1){
                                if(diffArrayInG[0] <= diffArrayInG[3]){
                                    selectedInG = 0;
                                }else{
                                    selectedInG = 3;
                                }
                            }else{
                                selectedInG = 0;
                            }

                        }
                    }
                }else{
                    if(diffValidInG[1] == 1){
                        if(diffValidInG[2] == 1){
                            if(diffValidInG[3] == 1){
                                if(diffArrayInG[1] <= diffArrayInG[2] && diffArrayInG[1] <= diffArrayInG[3]){
                                    selectedInG = 1;
                                }else if(diffArrayInG[2] <= diffArrayInG[3]){
                                    selectedInG = 2;
                                }else{
                                    selectedInG = 3;
                                }
                            }else{
                                 if(diffArrayInG[1] <= diffArrayInG[2]){
                                    selectedInG = 1;
                                }else{
                                    selectedInG = 2;
                                }
                            }
                        }else{
                            if(diffValidInG[3] == 1){
                                if(diffArrayInG[1] <= diffArrayInG[3]){
                                    selectedInG = 1;
                                }else{
                                    selectedInG = 3;
                                }
                            }else{
                                selectedInG = 1;
                            }
                        }
                    }else{
                        if(diffValidInG[2] == 1){
                            if(diffValidInG[3] == 1){
                                if(diffArrayInG[2] <= diffArrayInG[3]){
                                    selectedInG = 2;
                                }else{
                                    selectedInG = 3;
                                }
                            }else{
                                selectedInG = 2;
                            }
                        }else{
                            selectedInG = 3;
                        }
                    }
                }
                
                 target ->g = candidtateArrayInG[selectedInG]; 
            } 

           

            if(isEdge(x, y, 2, width_out, height_out) == 1){
                if(isEdge(x - 1, y, 2, width_out, height_out) == 0 && isEdge(x + 1, y, 2, width_out, height_out) == 0){
                    Pixel * targetLeft = bmp_pixel_at(res, x - 1, y, 2);
                    Pixel * targetRight = bmp_pixel_at(res, x + 1, y, 2); 
                     diffArrayInR[0] =  abs(targetLeft->r - targetRight->r);
                     candidtateArrayInR[0] = (targetLeft->r  + targetRight->r) / 2; 
                     diffValidInR[0] = (uint8_t)(1);
                }else{
                    diffArrayInR[0] =  (uint8_t)(0);
                     candidtateArrayInR[0] = (uint8_t)(0); 
                     diffValidInR[0] = (uint8_t)(0);
                }

                if(isEdge(x, y - 1, 2, width_out, height_out) == 0 && isEdge(x, y + 1, 2, width_out, height_out) == 0){
                    Pixel * targetUp = bmp_pixel_at(res, x, y - 1, 2);
                    Pixel * targetDown = bmp_pixel_at(res, x, y + 1, 2);
                     diffArrayInR[1] =  abs(targetUp->r - targetDown->r);
                     candidtateArrayInR[1] = (targetUp->r  + targetDown->r) / 2; 
                     diffValidInR[1] = (uint8_t)(1);
                }else{
                    diffArrayInR[1] =  (uint8_t)(0);
                    candidtateArrayInR[1] = (uint8_t)(0); 
                    diffValidInR[1] = (uint8_t)(0);
                }

                if(isEdge(x - 1, y - 1, 2, width_out, height_out) == 0 && isEdge(x + 1, y + 1, 2, width_out, height_out) == 0){
                    Pixel * targetLeftUp = bmp_pixel_at(res, x - 1, y - 1, 2);
                    Pixel * targetRightDown = bmp_pixel_at(res, x + 1, y + 1, 2);
                     diffArrayInR[2] =  abs(targetLeftUp->r - targetRightDown->r);
                     candidtateArrayInR[2] = (targetLeftUp->r  + targetRightDown->r) / 2; 
                     diffValidInR[2] = (uint8_t)(1);
                }else{
                    diffArrayInR[2] =  (uint8_t)(0);
                    candidtateArrayInR[2] = (uint8_t)(0); 
                    diffValidInR[2] = (uint8_t)(0);
                }

                if(isEdge(x - 1, y + 1, 2, width_out, height_out) == 0 && isEdge(x + 1, y - 1, 2, width_out, height_out) == 0){
                    Pixel * targetLeftDown = bmp_pixel_at(res, x - 1, y + 1, 2);
                    Pixel * targetRightUp = bmp_pixel_at(res, x + 1, y - 1, 2);
                     diffArrayInR[3] =  abs(targetLeftDown->r - targetRightUp->r);
                     candidtateArrayInR[3] = (targetLeftDown->r  + targetRightUp->r) / 2; 
                     diffValidInR[3] = (uint8_t)(1);
                     
                }else{
                     diffArrayInR[3] =  (uint8_t)(0);
                     candidtateArrayInR[3] = (uint8_t)(0); 
                     diffValidInR[3] = (uint8_t)(0);    
                }
            
             
             
                
                if(diffValidInR[0] == 1){
                    if(diffValidInR[1] == 1){
                        if(diffValidInR[2] == 1){
                            if(diffValidInR[3] == 1){
                                if(diffArrayInR[0] <= diffArrayInR[1] && diffArrayInR[0] <= diffArrayInR[2] && diffArrayInR[0] <= diffArrayInR[3]){
                                    selectedInR = 0;
                                }else if(diffArrayInR[1] <= diffArrayInR[2] && diffArrayInR[1] <= diffArrayInR[3]){
                                    selectedInR = 1;
                                }else if(diffArrayInR[2] <= diffArrayInR[3]){
                                    selectedInR = 2;
                                }else{
                                    selectedInR = 3;
                                }
                            }else{
                                if(diffArrayInR[0] <= diffArrayInR[1] && diffArrayInR[0] <= diffArrayInR[2]){
                                    selectedInR = 0;
                                }else if(diffArrayInR[1] <= diffArrayInR[2]){
                                    selectedInR = 1;
                                }else{
                                    selectedInR = 2;
                                }
                            }
                        }else{
                            if(diffValidInR[3] == 1){
                                if(diffArrayInR[0] <= diffArrayInR[1] && diffArrayInR[0] <= diffArrayInR[3]){
                                    selectedInR = 0;
                                }else if(diffArrayInR[1] <= diffArrayInR[3]){
                                    selectedInR = 1;
                                }else{
                                    selectedInR = 3;
                                }
                            }else {
                                if(diffArrayInR[0] <= diffArrayInR[1]){
                                    selectedInR = 0;
                                }else{
                                    selectedInR = 1;
                                }
                            }
                        }
                    }else{
                        if(diffValidInR[2] == 1){
                            if(diffValidInR[3]== 1){
                                if(diffArrayInR[0] <= diffArrayInR[2] && diffArrayInR[0] <= diffArrayInR[3]){
                                    selectedInR = 0;
                                }else if(diffArrayInR[2] <= diffArrayInR[3]){
                                    selectedInR = 2;
                                }else{
                                    selectedInR = 3;
                                }
                            }else{
                                if(diffArrayInR[0] <= diffArrayInR[2]){
                                    selectedInR = 0;
                                }else{
                                    selectedInR = 2;
                                }
                            }
                        }else{
                            if(diffValidInR[3] == 1){
                                if(diffArrayInR[0] <= diffArrayInR[3]){
                                    selectedInR = 0;
                                }else{
                                    selectedInR = 3;
                                }
                            }else{
                                selectedInR = 0;
                            }

                        }
                    }
                }else{
                    if(diffValidInR[1] == 1){
                        if(diffValidInR[2] == 1){
                            if(diffValidInR[3] == 1){
                                if(diffArrayInR[1] <= diffArrayInR[2] && diffArrayInR[1] <= diffArrayInR[3]){
                                    selectedInR = 1;
                                }else if(diffArrayInR[2] <= diffArrayInR[3]){
                                    selectedInR = 2;
                                }else{
                                    selectedInR = 3;
                                }
                            }else{
                                 if(diffArrayInR[1] <= diffArrayInR[2]){
                                    selectedInR = 1;
                                }else{
                                    selectedInR = 2;
                                }
                            }
                        }else{
                            if(diffValidInR[3] == 1){
                                if(diffArrayInR[1] <= diffArrayInR[3]){
                                    selectedInR = 1;
                                }else{
                                    selectedInR = 3;
                                }
                            }else{
                                selectedInR = 1;
                            }
                        }
                    }else{
                        if(diffValidInR[2] == 1){
                            if(diffValidInR[3] == 1){
                                if(diffArrayInR[2] <= diffArrayInR[3]){
                                    selectedInR = 2;
                                }else{
                                    selectedInR = 3;
                                }
                            }else{
                                selectedInR = 2;
                            }
                        }else{
                            selectedInR = 3;
                        }
                    }
                } 
            target ->r = candidtateArrayInR[selectedInR];
            } 
            
        }
    }
  return res;
    
}
  