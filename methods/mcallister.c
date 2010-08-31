/** mcallister.c
 *
 */

#define MAX_ITERATIONS 50

// These are the six elements of "target" and "x"
#define LEFT_LSTAR 0
#define LEFT_ASTAR 1
#define LEFT_BSTAR 2
#define RIGHT_LSTAR 3
#define RIGHT_ASTAR 4
#define RIGHT_BSTAR 5

#include <cv.h>
#include <levmar.h>

#include "color_tools.h"

void minimization_function(double *parameters, double *x, int m, int n, void *data) {
    x[LEFT_LSTAR] = CLstarLeft(&parameters[0]);
    x[LEFT_ASTAR] = CAstarLeft(&parameters[0]);
    x[LEFT_BSTAR] = CBstarLeft(&parameters[0]);
    x[RIGHT_LSTAR] = CLstarRight(&parameters[3]);
    x[RIGHT_ASTAR] = CAstarRight(&parameters[3]);
    x[RIGHT_BSTAR] = CBstarRight(&parameters[3]);
}

CvScalar method_combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    // These are the parameters that are varied
    double parameters[3] = {
        left_pixel.val[2], //R
        left_pixel.val[1], //G
        left_pixel.val[0]  //B
    };

    // Convert left_pixel and right_pixel to CIELab
    // These are the target values for the minimization
    double left_pixel_rgb[3] = {
        left_pixel.val[2], //R
        left_pixel.val[1], //G
        left_pixel.val[0]  //B
    };
    double right_pixel_rgb[3] = {
        right_pixel.val[2], //R
        right_pixel.val[1], //G
        right_pixel.val[0]  //B
    };
    double target[6] = {
        CLstarLeft(left_pixel_rgb),
        CAstarLeft(left_pixel_rgb),
        CBstarLeft(left_pixel_rgb),
        CLstarRight(right_pixel_rgb),
        CAstarRight(right_pixel_rgb),
        CBstarRight(right_pixel_rgb)
    };

    int numebr_of_iterations = dlevmar_dif(
        minimization_function,
        parameters,
        target,
        3, // length of parameters
        6, // length of target
        MAX_ITERATIONS,
        NULL, //TODO: opts
        NULL, //TODO: info
        NULL, //TODO: Working memory.
        NULL, // Covariance matrix
        NULL  // Extra data not needed
    );
    return cvScalar(
        parameters[2], //B
        parameters[1], //G
        parameters[0], //R
        0
    );
}

void method_init() {}
void method_free() {}
