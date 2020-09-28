#include <stdio.h>

#define ICOMIKI_COLOR_MIN_VALUE 0
#define ICOMIKI_COLOR_MAX_VALUE 255

int Icomiki_PrintColor(unsigned char red, unsigned char green, unsigned char blue) {
    printf("\e[48;2;%d;%d;%dm  \e[m", red, green, blue);
    return 0;
}
