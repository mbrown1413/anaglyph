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
//double opts[LM_OPTS_SZ];

unsigned long total_combines = 0;
double total_initial_gradient = 0;
unsigned long total_iterations = 0;
unsigned long terminate_reasons[8] = {0,0,0,0,0,0,0,0};
unsigned long function_evaluations = 0;
unsigned long jacobian_evaluations = 0;
unsigned long systems_solved = 0;

void minimization_function(float *parameters, float *x, int m, int n, void *data) {
    x[LEFT_LSTAR] = CLstarLeft(&parameters[0]);
    x[LEFT_ASTAR] = CAstarLeft(&parameters[0]);
    x[LEFT_BSTAR] = CBstarLeft(&parameters[0]);
    x[RIGHT_LSTAR] = CLstarRight(&parameters[0]);
    x[RIGHT_ASTAR] = CAstarRight(&parameters[0]);
    x[RIGHT_BSTAR] = CBstarRight(&parameters[0]);
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
        Lstarleft(left_pixel_rgb),
        Astarleft(left_pixel_rgb),
        Bstarleft(left_pixel_rgb),
        Lstarright(right_pixel_rgb),
        Astarright(right_pixel_rgb),
        Bstarright(right_pixel_rgb)
    };

    #ifdef MCALLISTER_STATISTICS
        float info[LM_INFO_SZ];
    #else
        float* info = NULL;
    #endif
    int number_of_iterations = slevmar_dif(
        minimization_function,
        parameters,
        target,
        3, // length of parameters
        6, // length of target
        MAX_ITERATIONS,
        NULL, //TODO: opts
        info,
        working_memory,
        NULL, // Covariance matrix
        NULL  // Extra data not needed
    );

    // Record info
    #ifdef MCALLISTER_STATISTICS
        total_combines++;
        total_initial_gradient += info[0];
        total_iterations += info[5];
        terminate_reasons[ (int)info[6] ] += 1;
        function_evaluations += info[7];
        jacobian_evaluations += info[8];
        systems_solved += info[9];
    #endif

    return cvScalar(
        parameters[2], //B
        parameters[1], //G
        parameters[0], //R
        0
    );
}

void method_init() {

    working_memory = (float*) malloc((LM_DIF_WORKSZ(3, 6))*sizeof(float));
    
    CIEmatricesLandR();

    // Verify the Jacobian
    /*
    float err[16];
    float p[3] = {127, 127, 127};
    dlevmar_chkjac(minimization_function, jacobian, p, 3, 6, NULL, err); 
    for(i=0; i<n; ++i) printf("Gradient %d, Error %g\n", i, err[i]);
    */

}
void method_free() {

    // Print recorded data
    #ifdef MCALLISTER_STATISTICS
        printf("\n");
        printf("Total pixel combines: %ld\n", total_combines);
        printf("Average initial gradient: %f\n", total_initial_gradient/total_combines);
        printf("Average iterations: %f\n", ((double)total_iterations)/total_combines);
        printf("Reasons for terminating:\n");
        printf("    1 - stopped by small gradient J^T e                                      %ld\n", terminate_reasons[1]);
        printf("    2 - stopped by small Dp                                                  %ld\n", terminate_reasons[2]);
        printf("    3 - stopped by itmax                                                     %ld\n", terminate_reasons[3]);
        printf("    4 - singular matrix. Restart from current p with increased \\mu           %ld\n", terminate_reasons[4]);
        printf("    5 - no further error reduction is possible. Restart with increased mu    %ld\n", terminate_reasons[5]);
        printf("    6 - stopped by small ||e||_2                                             %ld\n", terminate_reasons[6]);
        printf("    7 - stopped by invalid (i.e. NaN or Inf) \"func\" values; a user error     %ld\n", terminate_reasons[7]);
        printf("Average function evaluations: %f\n", ((double)function_evaluations)/total_combines);
        printf("Average jacobian evaluations: %f\n", ((double)jacobian_evaluations)/total_combines);
        printf("Average systems solved: %f\n", ((double)systems_solved)/total_combines);
    #endif

    free(working_memory);
}
