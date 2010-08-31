
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
