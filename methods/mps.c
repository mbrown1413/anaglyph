/** mps.c
 * The Modified Photoshop Method
 * Just like the photoshop method, except the left eye is converted to
 * greyscale first.  Uses NTSC luminance standard for greyscale.  
 */

#include <stdio.h>
#include <cv.h>

#include "interface.h"

CvScalar combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    double left_greyscale = 0.114*left_pixel.val[0] +
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

