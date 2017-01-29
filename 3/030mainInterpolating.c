/**
 * 030mainRastering.c
 * Duo Tao
 * Jan 7 2017
 * Main debugging module
 */
#include "030vector.c"
#include "030matrix.c"
#include "030triangle.c"

int main() {
	if (pixInitialize(512, 512, "Rasterizing a Rectangle") != 0) {
		return 1;
	} else {
		pixClearRGB(0.0, 0.0, 0.0);
        double a[2] = {100, 100};
        double b[2] = {200, 100};
        double c[2] = {200, 200};
        double rgb[3] = {1, 1, 1};
        double alpha[3] = {1, 0, 0};
        double beta[3] = {0, 1, 0};
        double gamma[3] = {0, 0, 1};
        triRender(a, b, c, rgb, alpha, beta, gamma);
        double a1[2] = {0, 0};
        double b1[2] = {100, 0};
        double c1[2] = {100, 100};
        triRender(a1, b1, c1, rgb, alpha, beta, gamma);
        double a2[2] = {200, 200};
        double b2[2] = {300, 200};
        double c2[2] = {300, 300};
        triRender(a2, b2, c2, rgb, alpha, beta, gamma);
        double a3[2] = {100, 0};
        double b3[2] = {200, 0};
        double c3[2] = {200, 100};
        triRender(a3, b3, c3, rgb, alpha, beta, gamma);
        double a4[2] = {0, 100};
        double b4[2] = {100, 100};
        double c4[2] = {100, 200};
        triRender(a4, b4, c4, rgb, alpha, beta, gamma);
		pixRun();
		return 0;
	}
}
