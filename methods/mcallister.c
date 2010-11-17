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
    float r = p[0];
    float g = p[1];
    float b = p[2];

    //Derivatives of LstarLeft
    float rgb_sum = (0.0018*b + 0.0118*g + 0.0876*r);
    if (rgb_sum/100 > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[0] = 116 * 0.00629095/cube_root_squared;
        jac[1] = 116 * 0.000847411/cube_root_squared;
        jac[2] = 116 * 0.000129266/cube_root_squared;
    } else {
        jac[0] = 116 * 0.682141/100;
        jac[1] = 116 * 0.0918866/100;
        jac[2] = 116 * 0.0140166/100;
    }

    //Derivatives of LstarRight
    rgb_sum = (0.0777*b +0.3088*g + 0.0176*r);
    if (rgb_sum/100 > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[9] = 116 * 0.00126394/cube_root_squared;
        jac[10] = 116 * 0.0221763/cube_root_squared;
        jac[11] = 116 * 0.005579/cube_root_squared;
    } else {
        jac[9] = 116 * 0.137051/100;
        jac[10] = 116 * 2.40463/100;
        jac[11] = 116 * 0.60505/100;
    }

    //Derivates of AstarLeft
    rgb_sum = 0.0018*b + 0.0118*g + 0.0876*r;
    if (rgb_sum/100 > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[3] = -0.00629095/cube_root_squared;
        jac[4] = -0.000847411/cube_root_squared;
        jac[5] = -0.000129266/cube_root_squared;
    } else {
        jac[3] = -0.682141/100;
        jac[4] = -0.0918866/100;
        jac[5] = -0.0140166/100;
    }

    rgb_sum = 0.0048*b + 0.0179*g + 0.184*r;
    if (0.00899734*rgb_sum > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[3] = 500 * (jac[3] + 0.0127566 / cube_root_squared);
        jac[4] = 500 * (jac[4] + 0.00124099 / cube_root_squared);
        jac[5] = 500 * (jac[5] + 0.000332781 / cube_root_squared);
    } else {
        jac[3] = 500 * (jac[3] + 1.43281 / 111.144);
        jac[4] = 500 * (jac[4] + 0.139387 / 111.144);
        jac[5] = 500 * (jac[5] + 0.0373776 / 111.144);
    }

    //Derivatives of AstarRight
    rgb_sum = 0.0777*b + 0.3088*g + 0.0176*r;
    if (rgb_sum/100 > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[12] = -0.00126394/cube_root_squared;
        jac[13] = -0.0221763/cube_root_squared;
        jac[14] = -0.00557999/cube_root_squared;
    } else {
        jac[12] = -0.137051/100;
        jac[13] = -2.40463/100;
        jac[14] = -0.60505/100;
    }

    rgb_sum = 0.1171*b + 0.1092*g + 0.0153*r;
    if (0.00899734*rgb_sum > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[12] = 500 * (jac[12] + 0.00899734 * 0.117895 / cube_root_squared);
        jac[13] = 500 * (jac[13] + 0.00899734 * 0.841444 / cube_root_squared);
        jac[14] = 500 * (jac[14] + 0.00899734 * 0.902318 / cube_root_squared);
    } else {
        jac[12] = 500 * (jac[12] + 0.119141 / 111.144);
        jac[13] = 500 * (jac[13] + 0.139387 / 111.144);
        jac[14] = 500 * (jac[14] + 0.0373776 / 111.144);
    }

    //Derivatives for BstarLeft
    rgb_sum = 0.0159*b + 0.0012*g + 0.0005*r;
    if (0.0284083*rgb_sum > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[6] = -0.00179013*0.0284083/cube_root_squared;
        jac[7] = -0.00429632*0.0284083/cube_root_squared;
        jac[8] = -0.00161718/cube_root_squared;
    } else {
        jac[6] = -0.0038935/35.201;
        jac[7] = -0.0093444/35.201;
        jac[8] = -0.123813/35.201;
    }

    rgb_sum = 0.0018*b + 0.0118*g + 0.0876*r;
    if (rgb_sum/100 > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[6] = 200*(jac[6] + 0.00629095/cube_root_squared);
        jac[7] = 200*(jac[7] + 0.000847411/cube_root_squared);
        jac[8] = 200*(jac[8] + 0.000129266/cube_root_squared);
    } else {
        jac[6] = 200*(jac[6] + 0.682141/100);
        jac[7] = 200*(jac[7] + 0.0918866/100);
        jac[8] = 200*(jac[8] + 0.0140166/100);
    }

    //Derivatives for BstarRight
    rgb_sum = 0.6546*b + 0.1016*g + 0.0201*r;
    if (0.0284083*rgb_sum > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[15] = -0.0719634*0.0284083/cube_root_squared;
        jac[16] = -0.363755*0.0284083/cube_root_squared;
        jac[17] = -2.34364*0.0284083/cube_root_squared;
    } else {
        jac[15] = -0.156519/35.201;
        jac[16] = -0.791159/35.201;
        jac[17] = -5.09737/35.201;
    }

    rgb_sum = 0.0777*b + 0.3088*g + 0.0176*r;
    if (rgb_sum/100 > 0.008856) {
        float cube_root_squared = pow(rgb_sum,2.0/3);
        jac[15] = 200*(jac[15] + 0.00126394/cube_root_squared);
        jac[16] = 200*(jac[16] + 0.0221763/cube_root_squared);
        jac[17] = 200*(jac[17] + 0.00557999/cube_root_squared);
    } else {
        jac[15] = 200*(jac[15] + 0.137051/100);
        jac[16] = 200*(jac[16] + 2.40463/100);
        jac[17] = 200*(jac[17] + 0.60505/100);
    }


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
    int number_of_iterations = slevmar_der(
        minimization_function,
        jacobian,
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
    for (int r=0; r < 255; r+=3) {for (int g=0; g < 255; g+=3) {for (int b=0; b < 255; b+=3) {
        float err[6];
        float p[3] = {g, g, g};
        slevmar_chkjac(minimization_function, jacobian, p, 3, 6, NULL, err);
        for(int i=0; i<6; ++i)
        {
            if (err[i] <= 0.8) {
                //printf("Gradient %d, (r,g,b)=(%d, %d, %d), Error %g\n", i, r,g,b, err[i]);
                printf("%d, %d, %d\n", r, g, b);
            }
        }
    }}}
    exit(0);
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
