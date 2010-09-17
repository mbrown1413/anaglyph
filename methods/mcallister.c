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

#include <stdio.h>
#include <cv.h>
#include <levmar.h>

#include "color_tools.h"

float* working_memory;

void minimization_function(float *parameters, float *x, int m, int n, void *data) {
    x[LEFT_LSTAR] = LCD_Lstar(&parameters[0]);
    x[LEFT_ASTAR] = LCD_Astar(&parameters[0]);
    x[LEFT_BSTAR] = LCD_Bstar(&parameters[0]);
    x[RIGHT_LSTAR] = LCD_Lstar(&parameters[0]);
    x[RIGHT_ASTAR] = LCD_Astar(&parameters[0]);
    x[RIGHT_BSTAR] = LCD_Bstar(&parameters[0]);
}

void jacobian(float *p, float *jac, int m, int n, void *data) {

}

CvScalar method_combine_pixels(CvScalar left_pixel, CvScalar right_pixel)
{
    // These are the parameters that are varied
    // They are the starting values and they are modified by levmar until the
    // function is minimized.
    float parameters[3] = {
        left_pixel.val[2], //R
        left_pixel.val[1], //G
        left_pixel.val[0]  //B
    };

    // Convert left_pixel and right_pixel to CIELab
    // These are the target values for the minimization
    float left_pixel_rgb[3] = {
        left_pixel.val[2], //R
        left_pixel.val[1], //G
        left_pixel.val[0]  //B
    };
    float right_pixel_rgb[3] = {
        right_pixel.val[2], //R
        right_pixel.val[1], //G
        right_pixel.val[0]  //B
    };
    float target[6] = {
        CLstarLeft(left_pixel_rgb),
        CAstarLeft(left_pixel_rgb),
        CBstarLeft(left_pixel_rgb),
        CLstarRight(right_pixel_rgb),
        CAstarRight(right_pixel_rgb),
        CBstarRight(right_pixel_rgb)
    };

    int number_of_iterations = slevmar_dif(
        minimization_function,
        parameters,
        target,
        3, // length of parameters
        6, // length of target
        MAX_ITERATIONS,
        NULL, //TODO: opts
        NULL, //TODO: info
        working_memory,
        NULL, // Covariance matrix
        NULL  // Extra data not needed
    );
    //printf("Iterations: %d\n", number_of_iterations);
    return cvScalar(
        parameters[2], //B
        parameters[1], //G
        parameters[0], //R
        0
    );
}

void method_init() {

    working_memory = (float*) malloc((LM_DIF_WORKSZ(3, 6))*sizeof(float));

    // Verify the Jacobian
    /*
    float err[16];
    float p[3] = {127, 127, 127};
    dlevmar_chkjac(minimization_function, jacobian, p, 3, 6, NULL, err); 
    for(i=0; i<n; ++i) printf("Gradient %d, Error %g\n", i, err[i]);
    */

}
void method_free() {
    free(working_memory);
}
