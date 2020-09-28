// standard library
#include <stdio.h>

// custom library
#include "icomiki.h"

int main() {

    for(int red=ICOMIKI_COLOR_MIN_VALUE; red<=ICOMIKI_COLOR_MAX_VALUE; red=red+8) {
        int blue = ICOMIKI_COLOR_MAX_VALUE - red;

        for(int green=ICOMIKI_COLOR_MIN_VALUE; green<=ICOMIKI_COLOR_MAX_VALUE; green=green+8) {
            Icomiki_PrintColor(red, green, blue);
        }
        printf("\n");
    }

    return 0;
}
