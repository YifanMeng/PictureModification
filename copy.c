#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time (because copy is a pixel-by-pixel transformation).
 *   2. Immediately write out each pixel.
 *
 * Note that this function should allocate space only for a single Pixel;
 * do *not* store more than one Pixel at a time, it isn't necessary here!
 */
void copy_filter(Bitmap *bmp) {
    Pixel *pixel = malloc(sizeof(Pixel));
    if (!pixel) {
        fprintf(stderr, "ERROR: malloc failed\n");
        exit(1);
    }

    // Just write out what's read in
    while (fread(pixel, 3, 1, stdin) == 1) {
        if(fwrite(pixel, 3, 1, stdout) != 1) {
            fprintf(stderr, "ERROR: fwrite failed\n");
            exit(1);
        }
    }

    free(pixel);
}

int main() {
    // Run the filter program with copy_filter to process the pixels.
    // You shouldn't need to change this implementation.
    run_filter(copy_filter, 1);
    return 0;
}
