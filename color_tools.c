#include <stdio.h>
#include <math.h>

#include "color_tools.h"

float Al[3][3] = {
    {5.42327, .807004, .047325},
    {2.70972, .50201, .0250529},
    {.0000550941, .000411221, .00240686}
};
float Ar[3][3] = {
    {.180431, 1.6395, 2.00309},
    {.448214, 6.31551, 1.35757},
    {.289201, 2.3925, 11.062}
};
float CIEmatrix[3][3] = {
    {11.6638, 8.3959, 4.65843},
    {7.10807, 16.6845, 2.45008},
    {.527874, 3.79124, 24.0604}
};

float Xn = 111.144;
float Yn = 100.0;
float Zn = 35.201;

float CIEmatrixL[3][3];
float CIEmatrixR[3][3];


void mult_matrices(float a[][3], float b[3], float result[3])
{
    result[0] = a[0][0] * b[0] +
                a[0][1] * b[1] +
                a[0][2] * b[2];
    result[1] = a[1][0] * b[0] +
                a[1][1] * b[1] +
                a[1][2] * b[2];
    result[2] = a[2][0] * b[0] +
                a[2][1] * b[1] +
                a[2][2] * b[2];
}

void print_matrix(float a[3])
{
   int i;
   for (i=0; i<3; i++)
   {
            //printf("%f\n", a[i]);
   }
}

/* CIEL*a*b* color transformation equations
* Xn, Yn, Zn are tristimulous values of reference white.
*The values used in the CIEmatrix, Al, and Ar are the LCD values
from the Uniform Metric paper
* The Al and Ar matrices give the set of values to multiply the rgb
values from the picture by to get the correct filter colors for the
left and right eye views
*/

void CIEmatricesLandR()
{
    float CIEleftconstant[3];
    float CIErightconstant[3];
    float CIEmatrixrightconstant[3];
    float CIEmatrixleftconstant[3];
    float RGBwhite[3] = {255, 255, 255};

    mult_matrices(Al, RGBwhite, CIEleftconstant);
    mult_matrices(Ar, RGBwhite, CIErightconstant);

    mult_matrices(CIEmatrix, RGBwhite, CIEmatrixleftconstant);
    mult_matrices(CIEmatrix, RGBwhite, CIEmatrixrightconstant);

    //compute normalization values n2, ..., n6 so that that whites agree in
    //CIE color space
    float n[6];
    for(int i=0; i<3; i++){
        n[i] = CIEleftconstant[i] / CIEmatrixleftconstant[i];
    }
    for(int i=3; i<6; i++){
        n[i] = CIErightconstant[i-3] / CIEmatrixrightconstant[i-3];
    }

    //reset the CIE conversion for the left and right eye RGB values
    element_wise_multiply(CIEmatrix, n, CIEmatrixL);
    element_wise_multiply(CIEmatrix, &n[3], CIEmatrixR);

}

//element_wise_multiply
void element_wise_multiply(float matrix[][3], float vector[3], float result[][3])
{
    for (int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            result[i][j] = matrix[i][j] * vector[i];
        }
    }
}

float cielabf(float U, float Un)
{
    float returnval;
    float result = U/Un;
    //printf("result = %f\n", result);
    if(result > .008856) {
        returnval = pow(result,1.0/3);
    } else {
        returnval = (7.787*result+16.0/116);
    }
    //printf("cielabf result is %f\n", returnval);
    return returnval;
}

float Lstar(float x, float y, float z)
{
    float r = (116.0 * cielabf(y, Yn) - 16.0);
    //printf("Lstar is %f\n", r);
    return r;
}

float Astar(float x, float y, float z)
{
    float r = 500.0 * (cielabf(x, Xn) - cielabf(y, Yn));
    //printf("Astar is  %f\n", r);
    return r;
}

float Bstar(float x, float y, float z)
{
    return (200.0 * (cielabf(y, Yn) - cielabf(z, Zn)));
}

void xyz_to_lab(float xyz[], float lab[]) {
    float cielabf_y_Yn = cielabf(xyz[1], Yn);
    lab[0] = 116.0 * cielabf_y_Yn - 16.0;
    lab[1] = 500.0 * (cielabf(xyz[0], Xn) - cielabf_y_Yn);
    lab[2] = 200.0 * (cielabf_y_Yn - cielabf(xyz[2], Zn));
}

/**
 * Computes the lab coordinates of the given rgb value, not seen through any filter.
 *
 * Normalized by the CIEmatrixL for the left eye filter.  This is why there are
 * two different functions for rgb to lab without a filter.
 */
void rgb_to_lab_without_left_filter(float rgb[], float lab[]) {
    float cie_xyz[3];
    mult_matrices(CIEmatrixL, rgb, cie_xyz);
    xyz_to_lab(cie_xyz, lab);
}

/**
 * Computes the lab coordinates of the given rgb value, not seen through any filter.
 *
 * Normalized by the CIEmatrixR for the right eye filter.  This is why there
 * are two different functions for rgb to lab without a filter.
 */
void rgb_to_lab_without_right_filter(float rgb[], float lab[]) {
    float cie_xyz[3];
    mult_matrices(CIEmatrixR, rgb, cie_xyz);
    xyz_to_lab(cie_xyz, lab);
}

/**
 * Computes the L coordinate in lab space of the given rgb value, not seen
 * through any filter.
 *
 * Normalized by the CIEmatrixL for the left eye filter.
 */
float Lstarleft(float rgb[3])
{
    float CIEmatrixLeft[3];

    mult_matrices(CIEmatrixL, rgb, CIEmatrixLeft);
    /*for (int i=0; i<3; i++){
        for(int j=0; j<3; j++) {
            printf("%f ", CIEmatrixL[i][j]);
        }
        printf("\n");
    }*/
    //printf("RGB: %f,%f,%f\n", rgb[0], rgb[1], rgb[2]);
    float test = Lstar(CIEmatrixLeft[0], CIEmatrixLeft[1], CIEmatrixLeft[2]);
    //printf("Lstarleft is %f\n", test);
    return test;

}

/**
 * Computes the A coordinate in lab space of the given rgb value, not seen
 * through any filter.
 *
 * Normalized by the CIEmatrixL for the left eye filter.
 */
float Astarleft(float rgb[3])
{
    float CIEmatrixLeft[3];
    mult_matrices(CIEmatrixL, rgb, CIEmatrixLeft);
    float test = Astar(CIEmatrixLeft[0], CIEmatrixLeft[1], CIEmatrixLeft[2]);
    //printf("Astarleft is %f\n", test);
    return test;

}

/**
 * Computes the B coordinate in lab space of the given rgb value, not seen
 * through any filter.
 *
 * Normalized by the CIEmatrixL for the left eye filter.
 */
float Bstarleft(float rgb[3])
{
    float CIEmatrixLeft[3];
    mult_matrices(CIEmatrixL, rgb, CIEmatrixLeft);
    float test = Bstar(CIEmatrixLeft[0], CIEmatrixLeft[1], CIEmatrixLeft[2]);
    //printf("Bstarleft is %f\n", test);
    return test;
}

/**
 * Computes the L coordinate in lab space of the given rgb value, not seen
 * through any filter.
 *
 * Normalized by the CIEmatrixR for the right eye filter.
 */
float Lstarright(float rgb[3])
{
    float CIEmatrixRight[3];
    mult_matrices(CIEmatrixR, rgb, CIEmatrixRight);
    /*for (int i=0; i<3; i++){
        for(int j=0; j<3; j++) {
            printf("%f ", CIEmatrixR[i][j]);
        }
        printf("\n");
    }*/

    float test = Lstar(CIEmatrixRight[0], CIEmatrixRight[1], CIEmatrixRight[2]);
    //printf("Lstarright is %f\n", test);
    return test;
}

/**
 * Computes the A coordinate in lab space of the given rgb value, not seen
 * through any filter.
 *
 * Normalized by the CIEmatrixR for the right eye filter.
 */
float Astarright(float rgb[3])
{
    float CIEmatrixRight[3];
    mult_matrices(CIEmatrixR, rgb, CIEmatrixRight);
    return Astar(CIEmatrixRight[0], CIEmatrixRight[1], CIEmatrixRight[2]);

}

/**
 * Computes the B coordinate in lab space of the given rgb value, not seen
 * through any filter.
 *
 * Normalized by the CIEmatrixR for the right eye filter.
 */
float Bstarright(float rgb[3])
{
    float CIEmatrixRight[3];
    mult_matrices(CIEmatrixR, rgb, CIEmatrixRight);
    return Bstar(CIEmatrixRight[0], CIEmatrixRight[1], CIEmatrixRight[2]);

}

/**
 * Converts the given rgb color to CIEXYZ space, given the conversion matrix A.
 */
void CIE(float A[][3], float rgb[3], float result[3])
{
    mult_matrices(A, rgb, result);
}

/**
 * Computes the lab coordinates of the given rgb value, seen through the left filter.
 */
void rgb_to_lab_through_left_filter(float rgb[], float lab[]){
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    xyz_to_lab(CIELeft, lab);
}

/**
 * Computes the lab coordinates of the given rgb value, seen through the left filter.
 */
void rgb_to_lab_through_right_filter(float rgb[], float lab[]){
    float CIERight[3];
    CIE(Ar, rgb, CIERight);
    xyz_to_lab(CIERight, lab);
}

/**
 * Computes the L coordinate in Lab space of the given rgb pixel as seen
 * through the left filter.
 */
float CLstarLeft(float rgb[3])
{
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    return Lstar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

/**
 * Computes the A coordinate in Lab space of the given rgb pixel as seen
 * through the left filter.
 */
float CAstarLeft(float rgb[3])
{
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    return Astar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

/**
 * Computes the B coordinate in Lab space of the given rgb pixel as seen
 * through the left filter.
 */
float CBstarLeft(float rgb[3])
{
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    //printf("CIELeft is %f, %f, %f\n", CIELeft[0], CIELeft[1], CIELeft[2]);
    return Bstar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

/**
 * Computes the L coordinate in Lab space of the given rgb pixel as seen
 * through the right filter.
 */
float CLstarRight(float rgb[3])
{
    float CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Lstar(CIEright[0], CIEright[1], CIEright[2]);
}

/**
 * Computes the A coordinate in Lab space of the given rgb pixel as seen
 * through the right filter.
 */
float CAstarRight(float rgb[3])
{
    float CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Astar(CIEright[0], CIEright[1], CIEright[2]);
}

/**
 * Computes the B coordinate in Lab space of the given rgb pixel as seen
 * through the right filter.
 */
float CBstarRight(float rgb[3])
{
    float CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Bstar(CIEright[0], CIEright[1], CIEright[2]);
}

float LCD_Lstar(float rgb[3])
{
    float lcd_CIE[3];
    CIE(CIEmatrix, rgb, lcd_CIE);
    return Lstar(lcd_CIE[0], lcd_CIE[1], lcd_CIE[2]);
}

float LCD_Astar(float rgb[3])
{
    float lcd_CIE[3];
    CIE(CIEmatrix, rgb, lcd_CIE);
    return Astar(lcd_CIE[0], lcd_CIE[1], lcd_CIE[2]);
}

float LCD_Bstar(float rgb[3])
{
    float lcd_CIE[3];
    CIE(CIEmatrix, rgb, lcd_CIE);
    return Bstar(lcd_CIE[0], lcd_CIE[1], lcd_CIE[2]);
}
