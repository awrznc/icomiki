// standard library
#include <stdio.h>

// custom library
#include "icomiki.h"

int main() {

    // for(int red=ICOMIKI_COLOR_MIN_VALUE; red<=ICOMIKI_COLOR_MAX_VALUE; red=red+8) {
    //     int blue = ICOMIKI_COLOR_MAX_VALUE - red;

    //     for(int green=ICOMIKI_COLOR_MIN_VALUE; green<=ICOMIKI_COLOR_MAX_VALUE; green=green+8) {
    //         Icomiki_PrintColor(red, green, blue);
    //     }
    //     printf("\n");
    // }

    PNG_Object png_object = PNG_Initialize();
    PNG_load(png_object);

    // ImageHeader image_header;
    // FILE *fpr = fopen("poke.png", "rb");
    // fread(&image_header, sizeof(image_header), 1, fpr);
    // fclose(fpr);

    // printf("id = %ld\n", image_header.width);
    // printf("id = %ld\n", sizeof(image_header));

    return 0;
}
