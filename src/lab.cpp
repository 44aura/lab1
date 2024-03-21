#include <iostream>
#include "BMP.h"

int main() {
    Image pic = Image();
    pic.Reader("11.bmp");

    Image clockwise = pic.RotateClock();
    clockwise.Writer("clockwise.bmp");

    Image counterclockwise = pic.RotateCounterclock();
    counterclockwise.Writer("countercloack.bmp");

    Image gauss = clockwise.Apply_gaussian_blur(10);
    gauss.Writer("gaussfilter.bmp");

    return 0;
}
