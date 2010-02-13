#include <cv.h>

#include "interface.h"

CvScalar combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    return cvScalar(
        left_pixel.val[0],
        left_pixel.val[0],
        right_pixel.val[0],
        0
    );
}

