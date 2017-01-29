/**
 * 030triangle.c
 * Duo Tao
 * Jan 7 2017
 * Rasterizing a triangle with linear interpolated color
 */
#include <stdio.h>
#include <math.h>
#include "000pixel.h"

int iceil(double x) {
    return (int) ceil(x);
}

int ifloor(double x) {
    return (int) floor(x);
}

// calculate the color at x based on linear interpolation and save the color into the color vector
void getColorAtX(double a[2], double b[2], double c[2], double x[2], double rgb[3], double alpha[3], double beta[3], double gamma[3], double color[3]) {
    // solve for the positional constants p and q
    double ac[2];
    double ab[2];
    double ax[2];
    vecSubtract(2, c, a, ac);
    vecSubtract(2, b, a, ab);
    vecSubtract(2, x, a, ax);
    double mat[2][2] = {{ab[0], ac[0]}, {ab[1], ac[1]}};
    double invMat[2][2];
    mat22Invert(mat, invMat);
    double pq[2];
    mat221Multiply(invMat, ax, pq);

    // get the color at x
    double abColor[3];
    double acColor[3];
    vecSubtract(3, beta, alpha, abColor);
    vecSubtract(3, gamma, alpha, acColor);
    vecScale(3, pq[0], abColor, abColor);
    vecScale(3, pq[1], acColor, acColor);
    vecAdd(3, abColor, acColor, color);
    vecAdd(3, color, alpha, color);
    color[0] *= rgb[0];
    color[1] *= rgb[1];
    color[2] *= rgb[2];
}

void triRender(double a[2], double b[2], double c[2], double rgb[3], double alpha[3], double beta[3], double gamma[3]) {
    // rotate A to the left
    if (b[0] < a[0] && b[0] <= c[0]) {
        double temp0 = a[0];
        double temp1 = a[1];
        a[0] = b[0];
        a[1] = b[1];
        b[0] = c[0];
        b[1] = c[1];
        c[0] = temp0;
        c[1] = temp1;
    } else if (c[0] < a[0] && c[0] <= b[0]) {
        double temp0 = a[0];
        double temp1 = a[1];
        a[0] = c[0];
        a[1] = c[1];
        c[0] = b[0];
        c[1] = b[1];
        b[0] = temp0;
        b[1] = temp1;
    }

    // handling two remaining cases
    if (b[0] < c[0]) {
        // B is in the middle of A and C
        int left = iceil(a[0]);
        int mid = ifloor(b[0]);
        int right = ifloor(c[0]);
        // rasterizing the two parts divided by B
        if (a[0] != b[0]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[1] + (b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0]));
                int top = ifloor(a[1] + (c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double color[3];
                    double x[2] = {x0, x1};
                    getColorAtX(a, b, c, x, rgb, alpha, beta, gamma, color);
                    pixSetRGB(x0, x1, color[0], color[1], color[2]);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(b[1] + (c[1] - b[1]) / (c[0] - b[0]) * (x0 - b[0]));
            int top = ifloor(a[1] + (c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0]));
            for (int x1 = bot; x1 <= top; x1++) {
                double color[3];
                double x[2] = {x0, x1};
                getColorAtX(a, b, c, x, rgb, alpha, beta, gamma, color);
                pixSetRGB(x0, x1, color[0], color[1], color[2]);
            }
        }
    } else {
        // C is in the middle of A and B
        int left = iceil(a[0]);
        int mid = ifloor(c[0]);
        int right = ifloor(b[0]);
        // rasterizing the two parts divided by C
        if (a[0] != c[0]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[1] + (b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0]));
                int top = ifloor(a[1] + (c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double color[3];
                    double x[2] = {x0, x1};
                    getColorAtX(a, b, c, x, rgb, alpha, beta, gamma, color);
                    pixSetRGB(x0, x1, color[0], color[1], color[2]);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(a[1] + (b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0]));
            int top = iceil(b[1] + (c[1] - b[1]) / (c[0] - b[0]) * (x0 - b[0]));
            for (int x1 = bot; x1 <= top; x1++) {
                double color[3];
                double x[2] = {x0, x1};
                getColorAtX(a, b, c, x, rgb, alpha, beta, gamma, color);
                pixSetRGB(x0, x1, color[0], color[1], color[2]);
            }
        }
    }
}


