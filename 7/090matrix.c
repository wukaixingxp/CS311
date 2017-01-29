/**
 * 090matrix.c
 * Duo Tao and Kaixing Wu
 * Jan 19 2017
 */


/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(double m[3][3], double n[3][3], double mTimesN[3][3]) {
  	for (int i = 0; i < 3; i++) {
  		for (int j = 0; j < 3; j++) {
  			mTimesN[i][j] = m[i][0] * n[0][j] + m[i][1] * n[1][j] + m[i][2] * n[2][j];
  		}
  	}
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. */
void mat331Multiply(double m[3][3], double v[3], double mTimesV[3]) {
	for (int j = 0; j < 3; j++) {
		mTimesV[j] = m[j][0] * v[0] + m[j][1] * v[1] + m[j][2] * v[2];
	}
}
/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector (x, y). 
*/
void mat33Isometry(double theta, double x, double y, double isom[3][3]) {
	theta = PI / 180.0 * theta;
	double rot[3][3];
    rot[0][0] = cos(theta);
    rot[0][1] = - sin(theta);
    rot[0][2] = 0;

    rot[1][0] = sin(theta);
    rot[1][1] = cos(theta);
    rot[1][2] = 0;

    rot[2][2] = 1;
    rot[2][1] = 0;
    rot[2][0] = 0;
   	double trans[3][3] = {
   		{1, 0, x},
   		{0, 1, y},
   		{0, 0, 1}
   	};
   	mat333Multiply(trans, rot, isom);
}

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat33Print(double m[3][3]) {
	int i;
	for (i = 0; i < 3; i += 1)
		printf("%f    %f    %f\n", m[i][0], m[i][1], m[i][2]);
}








/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(double m[2][2]) {
	int i;
	for (i = 0; i < 2; i += 1)
		printf("%f    %f\n", m[i][0], m[i][1]);
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
double mat22Invert(double m[2][2], double mInv[2][2]) {
	double det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	if (det == 0) {
		return det;
	}
	double deti = 1 / det;
	mInv[0][0] = deti * m[1][1];
	mInv[0][1] = deti * -m[0][1];
	mInv[1][0] = deti * -m[1][0];
	mInv[1][1] = deti * m[0][0];
	return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output should not */
void mat221Multiply(double m[2][2], double v[2], double mTimesV[2]) {
	mTimesV[0] = m[0][0] * v[0] + m[0][1] * v[1];
	mTimesV[1] = m[1][0] * v[0] + m[1][1] * v[1];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(double col0[2], double col1[2], double m[2][2]) {
	col0[0] = m[0][0];
	col0[1] = m[1][0];
	col1[0] = m[0][1];
	col1[1] = m[1][1];
}

// int main() {
// 	double mat[3][3];
// 	double v[3][3] = {
// 		{1, 1, 2},
//    		{0, 1, 3},
//    		{0, 0, 1}
// 	};
// 	double n[3][3] = {
// 		{1, 1, 2},
//    		{2, 1, 1},
//    		{0, 2, 1}
// 	};
// 	//mat333Multiply(v,n,mat);
// 	mat33Isometry(45, 1, 2, mat);
// 	mat33Print(mat);
// 	// double res[2][2];
// 	// mat22Invert(mat, res);
// 	// mat22Print(res);

// 	// double vec[2] = {1, 2};
// 	// double res1[2];
// 	// mat221Multiply(mat, vec, res1);
// 	// printf("%f   %f\n", res1[0], res1[1]);

// 	// double col0[2];
// 	// double col1[2];
// 	// mat22Columns(col0, col1, mat);
// 	// printf("%f   %f\n", col0[0], col0[1]);
// 	// printf("%f   %f\n", col1[0], col1[1]);
// }

