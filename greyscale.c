#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


void greyscale_filter(Bitmap *bmp) {
    Pixel *pixel = malloc(sizeof(Pixel));
    if (!pixel) {
        fprintf(stderr, "ERROR: malloc failed\n");
        exit(1);
    }
    unsigned char value;

    // Make a new pixel with the right values, then write it out
    while (fread(pixel, 3, 1, stdin) == 1) {
        value = (pixel->blue + pixel->green + pixel->red) / 3;
        pixel->blue = value;
        pixel->green = value;
        pixel->red = value;
        if (fwrite(pixel, 3, 1, stdout) != 1) {
            fprintf(stderr, "ERROR: fwrite failed\n");
            exit(1);
        }
    }

    free(pixel);
}

int main() {
    run_filter(greyscale_filter, 1);
    return 0;
}
