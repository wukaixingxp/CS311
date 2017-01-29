#include <stdio.h>

/*** In general dimensions ***/

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

// int main() {
// 	double res[2];
// 	double c = 2;
// 	double v[2] = {1.5, 2.0};
// 	double w[2] = {3.0, 3.5};
// 	vecCopy(2, v, res);
// 	printVector(2, res);
// 	vecAdd(2, v, w, res);
// 	printVector(2, res);
// 	vecScale(2, c, w, res);
// 	printVector(2, res);
// 	vecSubtract(2, v, w, res);
// 	printVector(2, res);
// }
