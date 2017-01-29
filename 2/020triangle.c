/**
 * 020triangle.c
 * Duo Tao
 * Jan 6 2017
 * Rasterizing a triangle
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

void triRender(double a0, double a1, double b0, double b1, double c0, 
        double c1, double r, double g, double b) {
    // rotate A to the left
    if (b0 < a0 && b0 <= c0) {
        double temp0 = a0;
        double temp1 = a1;
        a0 = b0;
        a1 = b1;
        b0 = c0;
        b1 = c1;
        c0 = temp0;
        c1 = temp1;
    } else if (c0 < a0 && c0 <= b0) {
        double temp0 = a0;
        double temp1 = a1;
        a0 = c0;
        a1 = c1;
        c0 = b0;
        c1 = b1;
        b0 = temp0;
        b1 = temp1;
    }

    // handling two remaining cases
    if (b0 < c0) {
        // B is in the middle of A and C
        int left = iceil(a0);
        int mid = ifloor(b0);
        int right = ifloor(c0);
        // rasterizing the two parts divided by B
        if (a0 != b0) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a1 + (b1 - a1) / (b0 - a0) * (x0 - a0));
                int top = ifloor(a1 + (c1 - a1) / (c0 - a0) * (x0 - a0));
                for (int x1 = bot; x1 <= top; x1++) {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(b1 + (c1 - b1) / (c0 - b0) * (x0 - b0));
            int top = ifloor(a1 + (c1 - a1) / (c0 - a0) * (x0 - a0));
            for (int x1 = bot; x1 <= top; x1++) {
                pixSetRGB(x0, x1, r, g, b);
            }
        }
    } else {
        // C is in the middle of A and B
        int left = iceil(a0);
        int mid = ifloor(c0);
        int right = ifloor(b0);
        // rasterizing the two parts divided by C
        if (a0 != c0) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a1 + (b1 - a1) / (b0 - a0) * (x0 - a0));
                int top = ifloor(a1 + (c1 - a1) / (c0 - a0) * (x0 - a0));
                for (int x1 = bot; x1 <= top; x1++) {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
        for (int x0 = mid; x0 <= right; x0++) {
            int bot = iceil(a1 + (b1 - a1) / (b0 - a0) * (x0 - a0));
            int top = iceil(b1 + (c1 - b1) / (c0 - b0) * (x0 - b0));
            printf("%d %d\n", bot, top);
            for (int x1 = bot; x1 <= top; x1++) {
                pixSetRGB(x0, x1, r, g, b);
            }
        }
    }
}


