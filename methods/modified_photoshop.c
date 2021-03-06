/** mps.c
 * The Modified Photoshop Method
 * Just like the photoshop method, except the left eye is converted to
 * greyscale first.  Uses NTSC luminance standard for greyscale.  
 * 
 * Baised on Zhe Zhang's thesis: "An Application of Linear Optimization in
 * Anaglyph Stereo Image Rendering"
 */

#include <stdio.h>
#include <cv.h>

#include "interface.h"

CvScalar method_combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    float left_greyscale = 0.114*left_pixel.val[0] +
                           0.587*left_pixel.val[1] +
                           0.299*left_pixel.val[2];
    //printf("%f %f %f %f\n", left_pixel.val[0], left_pixel.val[1], left_pixel.val[2], left_greyscale);
    return cvScalar(
        left_greyscale,
        left_greyscale,
        right_pixel.val[2],
        0
    );
}

void method_init() {}
void method_free() {}
