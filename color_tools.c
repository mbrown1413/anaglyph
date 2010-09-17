#include <stdio.h>
#include <math.h>

#include "color_tools.h"

float Al[3][3] = {
    {0.1840, 0.0179, 0.0048}, 
    {0.0876, 0.0118, 0.0018}, 
    {0.0005, 0.0012, 0.0159}
};
float Ar[3][3] = {
    {0.0153, 0.1092, 0.1171}, 
    {0.0176, 0.3088, 0.0777}, 
    {0.0201, 0.1016, 0.6546}
};
float CIEmatrix[3][3] = {
    {0.4243, 0.3105, 0.1657}, 
    {0.2492, 0.6419, 0.1089}, 
    {0.0265, 0.1225, 0.8614}
};

float Xn = 111.144;
float Yn = 100.0;
float Zn = 35.201;

float CIEmatrixL[3][3];
float CIEmatrixR[3][3];


void mult_matrices(float a[][3], float b[3], float result[3])
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

float Astarleft(float rgb[3])
{
    float CIEmatrixLeft[3];
    mult_matrices(CIEmatrixL, rgb, CIEmatrixLeft);
    float test = Astar(CIEmatrixLeft[0], CIEmatrixLeft[1], CIEmatrixLeft[2]); 
    //printf("Astarleft is %f\n", test);
    return test;

}

float Bstarleft(float rgb[3])
{
    float CIEmatrixLeft[3];
    mult_matrices(CIEmatrixL, rgb, CIEmatrixLeft);
    float test = Bstar(CIEmatrixLeft[0], CIEmatrixLeft[1], CIEmatrixLeft[2]);
    //printf("Bstarleft is %f\n", test);
    return test;
}

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

float Astarright(float rgb[3])
{
    float CIEmatrixRight[3];
    mult_matrices(CIEmatrixR, rgb, CIEmatrixRight);
    return Astar(CIEmatrixRight[0], CIEmatrixRight[1], CIEmatrixRight[2]);

}

float Bstarright(float rgb[3])
{
    float CIEmatrixRight[3];
    mult_matrices(CIEmatrixR, rgb, CIEmatrixRight);
    return Bstar(CIEmatrixRight[0], CIEmatrixRight[1], CIEmatrixRight[2]);

}

void CIE(float A[][3], float rgb[3], float result[3])
{
    mult_matrices(A, rgb, result);
}

float CLstarLeft(float rgb[3]) 
{
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    return Lstar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

float CAstarLeft(float rgb[3])
{
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    return Astar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

float CBstarLeft(float rgb[3])
{
    float CIELeft[3];
    CIE(Al, rgb, CIELeft);
    //printf("CIELeft is %f, %f, %f\n", CIELeft[0], CIELeft[1], CIELeft[2]);
    return Bstar(CIELeft[0], CIELeft[1], CIELeft[2]);
}

float CLstarRight(float rgb[3]) 
{
    float CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Lstar(CIEright[0], CIEright[1], CIEright[2]);
}

float CAstarRight(float rgb[3])
{
    float CIEright[3];
    CIE(Ar, rgb, CIEright);
    return Astar(CIEright[0], CIEright[1], CIEright[2]);
}

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
