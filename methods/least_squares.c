/** least_squares.c
 * Least Squares Method
 * Uses a least squares approximation in CIE color space.
 * 
 * Baised on this paper by David McAllister:
 *     "Computing Anaglyphs using Least Squares Approximation in CIE Color
 *     Space"
 * and baised on Zhe Zhang's thesis:
 *     "An Application of Linear Optimization in Anaglyph Stereo Image
 *     Rendering"
 * 
 * For each pixel, this matrix calculation is done:
 *     pl*Cl + pr*Cr
 * where Cr and Cl are the rgb vectors of the left and right images.  To make
 * the calculations simpler, they are done in this code as so:
 *     p*[Cl|Cr]
 *     where p=[pl|pr]
 * The matrix p is defined as a constant below.
 *
 */

#include <cv.h>

#include "interface.h"

// The matrix p
// See above about the calculation for how this matrix is used.
// These numbers came from "Computing Anaglyphs using Least Squares
// Approximation in CIE Color Space", by David McAllister.
static const float p[3][6] = {
    { 0.4155,  0.4710,  0.1670,   -0.0109, -0.0365, -0.0060},
    {-0.0458, -0.0484, -0.0258,    0.3756,  0.7333,  0.0111},
    {-0.0545, -0.0614,  0.0128,   -0.0651, -0.1286,  1.2968},
};

float clip(float num)
{
    if (num < 0) return 0;
    else if (num > 255) return 255;
    else return num;
}

CvScalar method_combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    // These each stand for left blue, right blue, etc.
    float lb = left_pixel.val[0];
    float lg = left_pixel.val[1];
    float lr = left_pixel.val[2];
    float rb = right_pixel.val[0];
    float rg = right_pixel.val[1];
    float rr = right_pixel.val[2];

    CvScalar result = cvScalar(
        // Remember that OpenCV likes to use BGR
        p[2][0]*lr+p[2][1]*lg+p[2][2]*lb + p[2][3]*rr+p[2][4]*rg+p[2][5]*rb,//B
        p[1][0]*lr+p[1][1]*lg+p[1][2]*lb + p[1][3]*rr+p[1][4]*rg+p[1][5]*rb,//G
        p[0][0]*lr+p[0][1]*lg+p[0][2]*lb + p[0][3]*rr+p[0][4]*rg+p[0][5]*rb,//R
        0
    );

    // Clipping
    result.val[0] = clip(result.val[0]);
    result.val[1] = clip(result.val[1]);
    result.val[2] = clip(result.val[2]);

    return result;
}

void method_init() {}
void method_free() {}
