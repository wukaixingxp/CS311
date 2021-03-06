
// #include <math.h>
// #include <stdio.h>
// #include <stdarg.h>
// #define PI 3.1415926535
// #include "100vector.c"

/**
 * 130matrix.c
 * Kaixing Wu
 * Jan 28 2017
 */
void mat33Identity(GLdouble m[3][3]){
	for (int i = 0; i < 3; i++) {
  		for (int j = 0; j < 3; j++) {
  			m[i][j] = 0.0;
	}
}
	m[0][0] = 1.0;
	m[1][1] = 1.0;
	m[2][2] = 1.0;
}

void mat44Identity(GLdouble m[4][4]){
	for (int i = 0; i < 4; i++) {
  		for (int j = 0; j < 4; j++) {
  			m[i][j] = 0.0;
	}
}
	m[0][0] = 1.0;
	m[1][1] = 1.0;
	m[2][2] = 1.0;
	m[3][3] = 1.0;

}
/* We want to pass matrices into OpenGL, but there are two obstacles. First, 
our matrix library uses GLdouble matrices, but OpenGL 2.x expects GLfloat 
matrices. Second, C matrices are implicitly stored one-row-after-another, while 
OpenGL expects matrices to be stored one-column-after-another. This function 
plows through both of those obstacles. */
void mat44OpenGL(GLdouble m[4][4], GLfloat openGL[4][4]) {
	for (int i = 0; i < 4; i += 1)
		for (int j = 0; j < 4; j += 1)
			openGL[i][j] = m[j][i];
} 

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(GLdouble m[3][3], GLdouble n[3][3], GLdouble mTimesN[3][3]) {
  	for (int i = 0; i < 3; i++) {
  		for (int j = 0; j < 3; j++) {
  			mTimesN[i][j] = m[i][0] * n[0][j] + m[i][1] * n[1][j] + m[i][2] * n[2][j];
  		}
  	}
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. */
void mat331Multiply(GLdouble m[3][3], GLdouble v[3], GLdouble mTimesV[3]) {
	for (int j = 0; j < 3; j++) {
		mTimesV[j] = m[j][0] * v[0] + m[j][1] * v[1] + m[j][2] * v[2];
	}
}
/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector (x, y). 
*/
void mat33Isometry(GLdouble theta, GLdouble x, GLdouble y, GLdouble isom[3][3]) {
	GLdouble rot[3][3];
    rot[0][0] = cos(theta);
    rot[0][1] = - sin(theta);
    rot[0][2] = 0;

    rot[1][0] = sin(theta);
    rot[1][1] = cos(theta);
    rot[1][2] = 0;

    rot[2][2] = 1;
    rot[2][1] = 0;
    rot[2][0] = 0;
   	GLdouble trans[3][3] = {
   		{1, 0, x},
   		{0, 1, y},
   		{0, 0, 1}
   	};
   	mat333Multiply(trans, rot, isom);
}

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat33Print(GLdouble m[3][3]) {
	int i;
	for (i = 0; i < 3; i += 1)
		printf("%f    %f    %f\n", m[i][0], m[i][1], m[i][2]);
}

void mat44Print(GLdouble m[4][4]) {
	int i;
	for (i = 0; i < 4; i += 1)
		printf("%f    %f    %f    %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
}

/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(GLdouble m[2][2]) {
	int i;
	for (i = 0; i < 2; i += 1)
		printf("%f    %f\n", m[i][0], m[i][1]);
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
GLdouble mat22Invert(GLdouble m[2][2], GLdouble mInv[2][2]) {
	GLdouble det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	if (det == 0) {
		return det;
	}
	GLdouble deti = 1 / det;
	mInv[0][0] = deti * m[1][1];
	mInv[0][1] = deti * -m[0][1];
	mInv[1][0] = deti * -m[1][0];
	mInv[1][1] = deti * m[0][0];
	return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output should not */
void mat221Multiply(GLdouble m[2][2], GLdouble v[2], GLdouble mTimesV[2]) {
	mTimesV[0] = m[0][0] * v[0] + m[0][1] * v[1];
	mTimesV[1] = m[1][0] * v[0] + m[1][1] * v[1];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(GLdouble col0[2], GLdouble col1[2], GLdouble m[2][2]) {
	col0[0] = m[0][0];
	col0[1] = m[1][0];
	col1[0] = m[0][1];
	col1[1] = m[1][1];
}
/* Given a length-1 3D vector axis and an angle theta (in radians), builds the 
rotation matrix for the rotation about that axis through that angle. Based on 
Rodrigues' rotation formula R = I + (sin theta) U + (1 - cos theta) U^2. */
void mat33AngleAxisRotation(GLdouble theta, GLdouble axis[3], GLdouble rot[3][3]){
	GLdouble U[3][3] = { 
		{0, -axis[2], axis[1]},
		{axis[2], 0, -axis[0]},
		{-axis[1], axis[0], 0}
	};
	GLdouble I[3][3] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};
	GLdouble Us[3][3];
	mat333Multiply(U, U, Us);
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			rot[i][j] =  I[i][j] + (sin(theta) * U[i][j]) + ((1 - cos(theta))* Us[i][j]);	
		}
	}
}

void mat33transpose(GLdouble m[3][3], GLdouble mT[3][3]){
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			mT[i][j] = m[j][i];
		}
	}
}

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. Builds 
the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(GLdouble u[3], GLdouble v[3], GLdouble a[3], GLdouble b[3], 
        GLdouble rot[3][3]){
	GLdouble w[3];
	GLdouble p[3];
	GLdouble R[3][3];
	GLdouble Y[3][3];
	vec3Cross(u, v, w);
	vec3Cross(a, b, p);
	for (int i = 0; i < 3; i++){
		R[i][0] = u[i];
		R[i][1] = v[i];
		R[i][2] = w[i];
		Y[i][0] = a[i];
		Y[i][1] = b[i];
		Y[i][2] = p[i];
	}
	GLdouble RT[3][3];
	mat33transpose(R, RT);
	mat333Multiply(Y, RT, rot);
}

/* Multiplies m by n, placing the answer in mTimesN. */
void mat444Multiply(GLdouble m[4][4], GLdouble n[4][4], GLdouble mTimesN[4][4]){
  	for (int i = 0; i < 4; i++) {
  		for (int j = 0; j < 4; j++) {
  			mTimesN[i][j] = m[i][0] * n[0][j] + m[i][1] * n[1][j] + m[i][2] * n[2][j] + m[i][3] * n[3][j];
  		}
  	}
}

/* Multiplies m by v, placing the answer in mTimesV. */
void mat441Multiply(GLdouble m[4][4], GLdouble v[4], GLdouble mTimesV[4]) {
for (int j = 0; j < 4; j++) {
		mTimesV[j] = m[j][0] * v[0] + m[j][1] * v[1] + m[j][2] * v[2] + m[j][3] * v[3];
	}
}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]) {
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			isom[i][j] = rot[i][j];
		}
	}
	isom[3][0] = 0;
	isom[3][1] = 0;
	isom[3][2] = 0; 
	isom[3][3] = 1;
	isom[0][3] = trans[0];
	isom[1][3] = trans[1];
	isom[2][3] = trans[2];
}

/* Given a rotation and translation, forms the 4x4 homogeneous matrix 
representing the inverse translation followed in time by the inverse rotation. 
That is, the isom produced by this function is the inverse to the isom 
produced by mat44Isometry on the same inputs. */
void mat44InverseIsometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]){
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			isom[i][j] = rot[j][i];
		}
	}
	isom[3][0] = 0;
	isom[3][1] = 0;
	isom[3][2] = 0; 
	isom[3][3] = 1;
	GLdouble rotT[3][3];
	GLdouble temp[3];
	mat33transpose(rot, rotT);
	mat331Multiply(rotT, trans, temp);
	isom[0][3] = -temp[0];
	isom[1][3] = -temp[1];
	isom[2][3] = -temp[2];
}

/* Builds a 4x4 matrix representing orthographic projection with a boxy viewing 
volume [left, right] x [bottom, top] x [far, near]. That is, on the near plane 
the box is the rectangle R = [left, right] x [bottom, top], and on the far 
plane the box is the same rectangle R. Keep in mind that 0 > near > far. Maps 
the viewing volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Orthographic(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4]){
  	for (int i = 0; i < 4; i++) {
  		for (int j = 0; j < 4; j++) {
  			proj[i][j] = 0;
  		}
  	}
	proj[0][0] = 2/(right - left);
	proj[1][1] = 2/(top - bottom);
	proj[2][2] = -2/(near - far);
	proj[2][3] = -(near + far)/(far - near);
	proj[0][3] = (right + left)/(left - right);
	proj[1][3] = (top + bottom)/(bottom - top);
	proj[3][3] = 1;
}
/* Builds a 4x4 matrix that maps a projected viewing volume 
[-1, 1] x [-1, 1] x [-1, 1] to screen [0, w - 1] x [0, h - 1] x [-1, 1]. */
void mat44Viewport(GLdouble width, GLdouble height, GLdouble view[4][4]){
	for (int i = 0; i < 4; i++) {
  		for (int j = 0; j < 4; j++) {
  			view[i][j] = 0;
  		}
  	}
  	view[0][0] = (width - 1)/ 2;
  	view[0][3] = (width - 1)/ 2;
  	view[1][1] = (height - 1)/ 2;
  	view[1][3] = (height - 1)/ 2;
  	view[2][2] = 1;
  	view[3][3] = 1;
}
/* Builds a 4x4 matrix representing perspective projection. The viewing frustum 
is contained between the near and far planes, with 0 > near > far. On the near 
plane, the frustum is the rectangle R = [left, right] x [bottom, top]. On the 
far plane, the frustum is the rectangle (far / near) * R. Maps the viewing 
volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Perspective(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
         GLdouble far, GLdouble near, GLdouble proj[4][4]){
	for (int i = 0; i < 4; i++) {
  		for (int j = 0; j < 4; j++) {
  			proj[i][j] = 0;
  		}
  	}
	proj[0][0] = -(2 * near) / (right - left);
	proj[1][1] = -(2 * near) / (top - bottom);
	proj[2][2] = -(near + far) / (far - near);
	proj[0][2] = (right + left) / (right - left);
	proj[1][2] = (top + bottom) / (top - bottom); 
	proj[3][2] =  -1;
	proj[2][3] =  -(2 * near * far) / (near - far);	
	
}
// int main() {

// 	GLdouble mat[4][4];
// 	GLdouble v1[3][3] = {
// 		{1, 0, 0},
// 		{1, 0, 0},
// 		{1, 0, 0}
// 	};
// 	GLdouble v2[3][3] = {
// 		{1, 0, 0},
// 		{0, 1, 0},
// 		{0, 0, 1}
// 	};
// 	GLdouble n1[3] = {0, 0, 0};
// 	GLdouble n2[3] = {0, 0, 1};
// 	mat44InverseIsometry(v1, n1 , mat);
// 	mat44Print(mat);
// }
// 	GLdouble v[3] = {
// 		1, 0, 1
// 	};
// 	GLdouble isom[4][4];
// 	GLdouble res[3];
// 	GLdouble vec[3] = {1, 0, 0};
// 	//vecUnit(3, v, v);
// 	//mat333Multiply(v,n,mat);
// 	mat33AngleAxisRotation(30, v, mat);
// 	printf("\n");
// 	mat33Print(mat);
// 	mat44InverseIsometry(mat,v,isom);
// 	mat44Print(isom);
	// mat331Multiply(isom, vec, res);
	// printVector(3, res);
	// GLdouble res[2][2];
	// mat22Invert(mat, res);
	// mat22Print(res);

	// GLdouble vec[2] = {1, 2};
	// GLdouble res1[2];
	// mat221Multiply(mat, vec, res1);
	// printf("%f   %f\n", res1[0], res1[1]);

	// GLdouble col0[2];
	// GLdouble col1[2];
	// mat22Columns(col0, col1, mat);
	// printf("%f   %f\n", col0[0], col0[1]);
	// printf("%f   %f\n", col1[0], col1[1]);
// }

