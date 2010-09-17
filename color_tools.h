
void mult_matrices(float a[][3], float b[3], float result[3]);
void print_matrix(float a[3]);
void CIEmatricesLandR();
void element_wise_multiply(float matrix[][3], float vector[3], float result[][3]);
void CIELab_A_matrixes(float Al[][3], float Ar[][3], float CIEmatrix[][3]);
float cielabf(float U, float Un);
float Lstar(float x, float y, float z); 
float Astar(float x, float y, float z);
float Bstar(float x, float y, float z); 

float LCD_Lstar(float rgb[3]);
float LCD_Astar(float rgb[3]);
float LCD_Bstar(float rgb[3]);

float CLstarRight(float rgb[3]);
float CAstarRight(float rgb[3]);
float CBstarRight(float rgb[3]);
float CLstarLeft(float rgb[3]);
float CAstarLeft(float rgb[3]);
float CBstarLeft(float rgb[3]);

float Lstarright(float rgb[3]);
float Astarright(float rgb[3]);
float Bstarright(float rgb[3]);
float Lstarleft(float rgb[3]);
float Astarleft(float rgb[3]);
float Bstarleft(float rgb[3]);
