
/**
 * 100vector.c
 * Duo Tao and Kaixing Wu
 * Jan 22 2017
 */

/* Returns the dot product of the dim-dimensional vectors v and w. */
double vecDot(int dim, double v[], double w[]) {
	double sum = 0;
	for (int i = 0; i < dim; i++) {
		sum += v[i] * w[i];
	}
	return sum;
}

/* Returns the length of the dim-dimensional vector v. */
double vecLength(int dim, double v[]) {
	double sum = 0;
	for (int i = 0; i < dim; i++) {
		sum += v[i] * v[i];
	}
	return sqrt(sum);
}

/* Returns the length of the dim-dimensional vector v. If the length is 
non-zero, then also places a scaled version of v into the dim-dimensional 
vector unit, so that unit has length 1. */
double vecUnit(int dim, double v[], double unit[]) {
	double len = vecLength(dim, v);
	if (len != 0) {
		for (int i = 0; i < dim; i++) {
			unit[i] = v[i] / len;
		}
	}
	return len;
}

/* Computes the cross product of the 3-dimensional vectors v and w, and places 
it into vCrossW. */
void vec3Cross(double v[3], double w[3], double vCrossW[3]) {
	vCrossW[0] = v[1] * w[2] - v[2] * w[1];
	vCrossW[1] = v[2] * w[0] - v[0] * w[2];
	vCrossW[2] = v[0] * w[1] - v[1] * w[0];
}

/* Computes the 3-dimensional vector v from its spherical coordinates. 
rho >= 0.0 is the radius. 0 <= phi <= pi is the co-latitude. -pi <= theta <= pi 
is the longitude or azimuth. */
void vec3Spherical(double rho, double phi, double theta, double v[3]) {
	v[0] = rho * sin(phi) * cos(theta);
	v[1] = rho * sin(phi) * sin(theta);
	v[2] = rho * cos(phi);
}

/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. */
void vecCopy(int dim, double v[], double copy[]) {
	for (int i = 0; i < dim; i++) {
		copy[i] = v[i];
	}
}

/* Adds the dim-dimensional vectors v and w. */
void vecAdd(int dim, double v[], double w[], double vPlusW[]) {
	for (int i = 0; i < dim; i++) {
		vPlusW[i] = v[i] + w[i];
	}
}

/* Subtracts the dim-dimensional vectors v and w. */
void vecSubtract(int dim, double v[], double w[], double vMinusW[]) {
	for (int i = 0; i < dim; i++) {
		vMinusW[i] = v[i] - w[i];
	}
}

/* Scales the dim-dimensional vector w by the number c. */
void vecScale(int dim, double c, double w[], double cTimesW[]) {
	for (int i = 0; i < dim; i++) {
		cTimesW[i] = c * w[i];
	}
}

void printVector(int dim, double w[]) {
	for (int i = 0; i < dim; i++) {
		printf("%f ", w[i]);
	}
	printf("\n");
}

/* Assumes that there are dim + 2 arguments, the last dim of which are doubles. 
Sets the dim-dimensional vector v to those doubles. */
void vecSet(int dim, double v[], ...) {
	va_list argumentPointer;
	va_start(argumentPointer, v);
	for (int i = 0 ; i < dim; i++) {
		v[i] = va_arg(argumentPointer, double);
	}
	va_end(argumentPointer);
}

// int main() {
// 	double v[3] = {1, 2, 3};
// 	double w[3] = {2, 3, 1};
// 	double res[3];
// 	int dim = 3;
// 	/* Returns the dot product of the dim-dimensional vectors v and w. */
// 	printf("%f\n", vecDot(dim, v, w));

// 	/* Returns the length of the dim-dimensional vector v. */
// 	printf("%f\n", vecLength(dim, v));

// 	 Returns the length of the dim-dimensional vector v. If the length is 
// 	non-zero, then also places a scaled version of v into the dim-dimensional 
// 	vector unit, so that unit has length 1. 
// 	vecUnit(dim, v, res);
// 	printVector(dim, res);

// 	/* Computes the cross product of the 3-dimensional vectors v and w, and places 
// 	it into vCrossW. */
// 	vec3Cross(v, w, res);
// 	printVector(dim, res);
// 	/* Computes the 3-dimensional vector v from its spherical coordinates. 
// 	rho >= 0.0 is the radius. 0 <= phi <= pi is the co-latitude. -pi <= theta <= pi 
// 	is the longitude or azimuth. */
// 	vec3Spherical(1, 45, 30, res);
// 	printVector(dim, res);
// }
