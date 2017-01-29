/**
 * 090triangle.c
 * Duo Tao and Kaixing Wu
 * Jan 19 2017
 */

int iceil(double x) {
    return (int) ceil(x);
}

int ifloor(double x) {
    return (int) floor(x);
}

// calculate the color at x based on linear interpolation and save the color into the color vector
//void getColorAtX(double a[2], double b[2], double c[2], double x[2], texTexture *tex, double alpha[2], double beta[2], double gamma[2]) {
void getColorAtX(double x[], renRenderer *ren, double unif[], texTexture *tex[], double a[], 
        double b[], double c[], double rgb[]) {
    double vary[ren->varyDim];
    vary[renVARYX] = x[0];
    vary[renVARYY] = x[1];

    // solve for the interpolating parameters p and q
    double ac[2];
    double ab[2];
    double ax[2];
    double st[2];
    vecSubtract(2, c, a, ac);
    vecSubtract(2, b, a, ab);
    vecSubtract(2, x, a, ax);
    double mat[2][2] = {{ab[0], ac[0]}, {ab[1], ac[1]}};
    double invMat[2][2];
    mat22Invert(mat, invMat);
    double pq[2];
    mat221Multiply(invMat, ax, pq);

    // get the interpolated texture coordinates at x
    double alphaT[2] = {a[renVARYS], a[renVARYT]};
    double betaT[2] = {b[renVARYS], b[renVARYT]};
    double gammaT[2] = {c[renVARYS], c[renVARYT]};
    double abTex[2];
    double acTex[2];
    vecSubtract(2, betaT, alphaT, abTex);
    vecSubtract(2, gammaT, alphaT, acTex);
    vecScale(2, pq[0], abTex, abTex);
    vecScale(2, pq[1], acTex, acTex);
    vecAdd(2, abTex, acTex, st);
    vecAdd(2, st, alphaT, st);
    vary[renVARYS] = st[0];
    vary[renVARYT] = st[1];

    // get the interpolated RGB at x
    double rgbC[3]; 
    double abColor[3];
    double acColor[3];
    double alphaC[3] = {a[renVARYR], a[renVARYG], a[renVARYB]};
    double betaC[3] = {b[renVARYR], b[renVARYG], b[renVARYB]};
    double gammaC[3] = {c[renVARYR], c[renVARYG], c[renVARYB]};
    vecSubtract(3, betaC, alphaC, abColor);
    vecSubtract(3, gammaC, alphaC, acColor);
    vecScale(3, pq[0], abColor, abColor);
    vecScale(3, pq[1], acColor, acColor);
    vecAdd(3, abColor, acColor, rgbC);
    vecAdd(3, rgbC, alphaC, rgbC);
    vary[renVARYR] = rgbC[0];
    vary[renVARYG] = rgbC[1];
    vary[renVARYB] = rgbC[2];

    // figure out the color with these and save it into rgb
    ren->colorPixel(ren, unif, tex, vary, rgb); 
}

// rotate A to the left
void vertexPreprocess(renRenderer *ren, double a[], double b[], double c[]) {
    double temp[ren->varyDim];
    if (b[renVARYX] < a[renVARYX] && b[renVARYX] <= c[renVARYX]) {
        vecCopy(ren->varyDim, a, temp);
        vecCopy(ren->varyDim, b, a);
        vecCopy(ren->varyDim, c, b);
        vecCopy(ren->varyDim, temp, c);
    } else if (c[renVARYX] < a[renVARYX] && c[renVARYX] <= b[renVARYX]) {
        vecCopy(ren->varyDim, a, temp);
        vecCopy(ren->varyDim, c, a);
        vecCopy(ren->varyDim, b, c);
        vecCopy(ren->varyDim, temp, b);
    }
}

void triRender(renRenderer *ren, double unif[], texTexture *tex[], double aVary[], double bVary[], double cVary[]) {
    double rgb[3]; // place to save the color at all positions x

    // preprocess
    double a[ren->varyDim];
    double b[ren->varyDim];
    double c[ren->varyDim];
    vecCopy(ren->varyDim, aVary, a);
    vecCopy(ren->varyDim, bVary, b);
    vecCopy(ren->varyDim, cVary, c);
    vertexPreprocess(ren, a, b, c);

    // handling two remaining cases
    if (b[renVARYX] < c[renVARYX]) {
        // B is in the middle of A and C
        int left = iceil(a[renVARYX]);
        int mid = ifloor(b[renVARYX]);
        int right = ifloor(c[renVARYX]);
        // rasterizing the two parts divided by B
        if (a[0] != b[0]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                int top = ifloor(a[renVARYY] + (c[renVARYY] - a[renVARYY]) / (c[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double x[2] = {x0, x1};
                    getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
            int top = ifloor(a[renVARYY] + (c[renVARYY] - a[renVARYY]) / (c[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
            for (int x1 = bot; x1 <= top; x1++) {
                double x[2] = {x0, x1};
                getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
            }
        }
    } else {
        // C is in the middle of A and B
        int left = iceil(a[renVARYX]);
        int mid = ifloor(c[renVARYX]);
        int right = ifloor(b[renVARYX]);
        // rasterizing the two parts divided by C
        if (a[0] != c[0]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                int top = ifloor(a[renVARYY] + (c[renVARYY] - a[renVARYY]) / (c[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double x[2] = {x0, x1};
                    getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
            int top = iceil(b[renVARYY] + (c[renVARYY] - b[renVARYY]) / (c[renVARYX] - b[renVARYX]) * (x0 - b[renVARYX]));
            for (int x1 = bot; x1 <= top; x1++) {
                double x[2] = {x0, x1};
                getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
            }
        }
    }
}


