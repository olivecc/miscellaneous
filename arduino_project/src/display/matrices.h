#ifndef ARD_CSWRK_MATRICES
#define ARD_CSWRK_MATRICES

void applyTransform(const double[3][3], double[3]);
void updateTransform(const double[3][3], double[3][3]);
void setToIdentity(double[3][3]);
void getRollTransform(double[3][3], double);
void getYawTransform(double[3][3], double);
void getPitchTransform(double[3][3], double);
void randomMatrix(double[3][3]);

#endif //ARD_CSWRK_MATRICES
