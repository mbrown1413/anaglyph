/** ps.c
 * The Photoshop Method
 * A simple RGB filter for each eye to determine the pixel color.
 */

#include <cv.h>

#include "interface.h"

CvScalar combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    return cvScalar(
        left_pixel.val[0],
        left_pixel.val[1],
        right_pixel.val[2],
        0
    );
}

