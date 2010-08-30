#include <stdio.h>
#include <math.h>

void mult_matrices(double a[][3], double b[3], double result[3]);
void print_matrix(double a[3]);
void element_wise_multiply(double matrix[][3], double vector[3], double result[][3]);
void CIELab_A_matrixes(double Al[][3], double Ar[][3], double CIEmatrix[][3]);
double cielabf(double U, double Un);
double Lstar(double x, double y, double z); 
double Astar(double x, double y, double z);
double Bstar(double x, double y, double z); 

double CLstarRight(double rgb[3]);
double CAstarRight(double rgb[3]);
double CBstarRight(double rgb[3]);
double CLstarLeft(double rgb[3]);
double CAstarLeft(double rgb[3]);
double CBstarLeft(double rgb[3]);

double Al[3][3] = {
    {0.1840, 0.0179, 0.0048}, 
    {0.0876, 0.0118, 0.0018}, 
    {0.0005, 0.0012, 0.0159}
};
double Ar[3][3] = {
    {0.0153, 0.1092, 0.1171}, 
    {0.0176, 0.3088, 0.0777}, 
    {0.0201, 0.1016, 0.6546}
};
double CIEmatrix[3][3] = {
    {0.4243, 0.3105, 0.1657}, 
    {0.2492, 0.6419, 0.1089}, 
    {0.0265, 0.1225, 0.8614}
};

double Xn = 111.144;
double Yn = 100.0;
double Zn = 35.201;

void mult_matrices(double a[][3], double b[3], double result[3])
{
    for (int i=0; i<3; i++) {
        result[i] = 0;
    }
    int i, j;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
     //           printf("a[%d][%d] = %f\n", i, j, a[i][j]);
       //         printf("b[%d] = %f\n", j, b[j]);
            result[i] += a[i][j] * b[j];
        }
   }
}

void print_matrix(double a[3])
{
   int i; 
   for (i=0; i<3; i++)
   {
            printf("%f\n", a[i]);
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

void CIEmatricesLandR(double CIEmatrixL[][3], double CIEmatrixR[][3])
{
    double CIEleftconstant[3];
    double CIErightconstant[3];
    double CIEmatrixconstant[3];
    double RGBwhite[3] = {255, 255, 255};

    mult_matrices(Al, RGBwhite, CIEleftconstant);
    mult_matrices(Ar, RGBwhite, CIErightconstant);

    mult_matrices(CIEmatrix, RGBwhite, CIEmatrixconstant);
    mult_matrices(Ar, RGBwhite, CIErightconstant);

    //compute normalization values n1, ..., n6 so that that whites agree in \
    //CIE color space
    double n[6];
    for(int i=0; i<3; i++){
        n[i] = CIEleftconstant[i] / CIEmatrixconstant[i];
    }
    for(int i=3; i<6; i++){
        n[i] = CIErightconstant[i] / CIEmatrixconstant[i];
    }

    //reset the CIE conversion for the left and right eye RGB values
    element_wise_multiply(CIEmatrix, n, CIEmatrixL);
    element_wise_multiply(CIEmatrix, &n[4], CIEmatrixR);

}

//element_wise_multiply
void element_wise_multiply(double matrix[][3], double vector[3], double result[][3])
{
    for (int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            result[i][j] = matrix[i][j] * vector[j];
        }
    }
}

double cielabf(double U, double Un)
{
    double returnval;
    double result = U/Un;
    printf("result = %f\n", result);
    if(result > .008856) {
        returnval = pow(result,1.0/3);
    } else {
        returnval = (7.787*result+16.0/116);
    }
    printf("cielabf result is %f\n", returnval);
    return returnval;
}

double Lstar(double x, double y, double z) 
{
    double r = (116.0 * cielabf(y, Yn) - 16.0);
    printf("Lstar is %f\n", r);
    return r;
}

double Astar(double x, double y, double z)
{
    double r = 500.0 * (cielabf(x, Xn) - cielabf(y, Yn));
    printf("Astar is  %f\n", r);
    return r;
}

double Bstar(double x, double y, double z) 
{
    return (200.0 * (cielabf(y, Yn) - cielabf(z, Zn)));
}

void CIE(double A[][3], double rgb[3], double result[3])
{
    mult_matrices(A, rgb, result);
}

double CLstarLeft(double rgb[3]) 
{
    double CIELeft[3];
    CIE(Al, rgb, CIELeft);
    return Lstar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

double CAstarLeft(double rgb[3])
{
    double CIELeft[3];
    CIE(Al, rgb, CIELeft);
    return Astar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

double CBstarLeft(double rgb[3])
{
    double CIELeft[3];
    CIE(Al, rgb, CIELeft);
    printf("CIELeft is %f, %f, %f\n", CIELeft[0], CIELeft[1], CIELeft[2]);
    return Bstar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

double CLstarRight(double rgb[3]) 
{
    double CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Lstar(CIEright[0], CIEright[1], CIEright[2]);
}

double CAstarRight(double rgb[3])
{
    double CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Astar(CIEright[0], CIEright[1], CIEright[2]);
}

double CBstarRight(double rgb[3])
{
    double CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Bstar(CIEright[0], CIEright[1], CIEright[2]);
}


