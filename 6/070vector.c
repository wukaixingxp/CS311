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

/* Assumes that there are dim + 2 arguments, the last dim of which are doubles. 
Sets the dim-dimensional vector v to those doubles. */
void vecSet(int dim, double v[], ...) {
	va_list argumentPointer;
	va_start(argumentPointer, v);
	for (int i = 0 ; i < dim; i++) {
		v[i] = va_arg(argumentPointer, double);
		printf("%f\n", v[i]);
	}
	va_end(argumentPointer);
}

// int main() {
// 	double v[3];
// 	vecSet(3, v, 1.0, 2.0, 2.5);
// 	printVector(3, v);
// }
