#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


void gaussian_blur_filter(Bitmap *bmp) {
    int width = bmp->width;
    int height = bmp->height;
    int row, col, i, j;
    Pixel new_pixel;
    Pixel *temp;

    // Allocate space for storing 3 rows of pixels
    Pixel **rows = malloc(sizeof(Pixel *) * 3);
    if (!rows) {
        fprintf(stderr, "ERROR: malloc failed\n");
        exit(1);
    }

    for (i = 0; i < 3; i++) {
        rows[i] = malloc(sizeof(Pixel) * width);
        if (!rows[i]) {
            fprintf(stderr, "ERROR: malloc failed\n");
            exit(1);
        }

        // Read in the first 3 rows
        for (j = 0; j < width; j++) {
            if(fread(rows[i] + j, 3, 1, stdin) != 1) {
                fprintf(stderr, "ERROR: fread failed\n");
                exit(1);
            }
        } 
    }
    for (row = 0; row < height; row++){
        for (col = 0; col < width; col++) {
            //left most pixel
            if (col == 0) { 
                new_pixel = apply_gaussian_kernel(rows[0], rows[1], rows[2]);
            //right most pixel
            } else if (col == (width - 1)) { 
                new_pixel = apply_gaussian_kernel(rows[0] + col - 2, 
                    rows[1] + col - 2, rows[2] + col - 2);
            //pixels in the middle
            } else { 
                new_pixel = apply_gaussian_kernel(rows[0] + col - 1, 
                    rows[1] + col - 1, rows[2] + col - 1);
            }
            if(fwrite(&new_pixel, 3, 1, stdout) != 1) {
                fprintf(stderr, "ERROR: fwrite failed\n");
                exit(1);
            }

        }
        //Read in a new line after processing every row, except for the first and
        //the last row
        //Exchange rows so that they point to the new top, middle, bottom rows
        if (row > 0 && row < (height - 2)) {
            temp = rows[0];
            rows[0] = rows[1];
            rows[1] = rows[2];
            rows[2] = temp;
            for (j = 0; j < width; j++) {
                if(fread(rows[2] + j, 3, 1, stdin) != 1) {
                    fprintf(stderr, "ERROR: fread failed,row:%d,j:%d\n",row,j);
                    exit(1);
                }
            }
        }
    }
    // Free the momery 
    for (i = 0; i < 3; i++) {
        free(rows[i]);
    }
    free(rows);
}

int main() {
    run_filter(gaussian_blur_filter, 1);
    return 0;
}
