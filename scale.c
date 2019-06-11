#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


void scale_filter(Bitmap *bmp) {
    int row, col, i, j, width, height, scale_factor;
    scale_factor = bmp->scale_factor;
    //The width and height of the current image are calculated
    width = bmp->width / scale_factor;
    height = bmp->height / scale_factor;
    Pixel *old_row = malloc(sizeof(Pixel) * width);
    if (!old_row) {
        fprintf(stderr, "ERROR: malloc failed\n");
        exit(1);
    }

    for (row = 0; row < height; row++){
        //Read in the old row
        if(fread(old_row, 3, width, stdin) != width) {
            fprintf(stderr, "ERROR: fread failed\n");
            exit(1);
        }
        //Write out the scaled version of it
        for (i = 0; i < scale_factor; i++) {
            for (col = 0; col < width; col++) {
                for (j = 0; j < scale_factor; j++) {
                    if(fwrite(old_row + col, 3, 1, stdout) != 1) {
                        fprintf(stderr, "ERROR: fwrite failed\n");
                        exit(1);
                    }
                }
            }
        }
    }
    free(old_row);
}

int main(int argc, char **argv){
    run_filter(scale_filter, strtol(argv[1], NULL, 10));
    return 0;
}
