/**
 * 110triangle.c
 * Duo Tao and Kaixing Wu
 * Jan 24 2017
 */

int iceil(double x) {
    return (int) ceil(x);
}

int ifloor(double x) {
    return (int) floor(x);
}

// calculate the color at x based on linear interpolation and save the color into the color vector
void colorX(double x[], renRenderer *ren, double unif[], texTexture *tex[], 
    double a[], double b[], double c[], double rgbz[]) {
    double vary[ren->varyDim];
    vary[renVARYX] = x[0];
    vary[renVARYY] = x[1];

    // solve for the interpolating parameters p and q
    double ac[3];
    double ab[3];
    double ax[3];
    double st[3];
    vecSubtract(3, c, a, ac);
    vecSubtract(3, b, a, ab);
    vecSubtract(3, x, a, ax);
    double mat[2][2] = {{ab[0], ac[0]}, {ab[1], ac[1]}};
    double invMat[2][2];
    mat22Invert(mat, invMat);
    double pq[2];
    mat221Multiply(invMat, ax, pq);

    // get the interpolated texture coordinates at x
    double alpha[3] = {a[renVARYS], a[renVARYT], a[renVARYZ]};
    double beta[3] = {b[renVARYS], b[renVARYT], b[renVARYZ]};
    double gamma[3] = {c[renVARYS], c[renVARYT], c[renVARYZ]};
    double alphaBeta[3];
    double alphaGamma[3];
    vecSubtract(3, beta, alpha, alphaBeta);
    vecSubtract(3, gamma, alpha, alphaGamma);
    vecScale(3, pq[0], alphaBeta, alphaBeta);
    vecScale(3, pq[1], alphaGamma, alphaGamma);
    vecAdd(3, alphaBeta, alphaGamma, st);
    vecAdd(3, st, alpha, st);
    vary[renVARYS] = st[0];
    vary[renVARYT] = st[1];
    vary[renVARYZ] = st[2];
    // figure out the color with these and save it into rgb
    ren->colorPixel(ren, unif, tex, vary, rgbz);
    if (depthGetZ(ren->depth, x[0], x[1]) < rgbz[3]) {
        pixSetRGB(x[0], x[1], rgbz[0], rgbz[1], rgbz[2]);
        depthSetZ(ren->depth, x[0], x[1], rgbz[3]);
    }
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

char validVert(double a[], double b[], double c[]) {
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]) > 0;
}

void triRender(renRenderer *ren, double unif[], texTexture *tex[], double aVary[], double bVary[], double cVary[]) {
    double rgbz[4]; // place to save the color at all positions x
    // printf("%f   %f    %f  a\n", aVary[0], aVary[1],aVary[2] );
    // printf("%f   %f    %f  b\n", bVary[0], bVary[1],bVary[2] );
    // printf("%f   %f    %f  c\n", cVary[0], cVary[1],cVary[2] );
    // preprocess
    double a[ren->varyDim];
    double b[ren->varyDim];
    double c[ren->varyDim];
    vecCopy(ren->varyDim, aVary, a);
    vecCopy(ren->varyDim, bVary, b);
    vecCopy(ren->varyDim, cVary, c);
    if (!validVert(a, b, c)) {
        return;
    }
    vertexPreprocess(ren, a, b, c);
    fflush(stdout);
    // handling two remaining cases
    if (b[renVARYX] < c[renVARYX]) {
        // B is in the middle of A and C
        int left = iceil(a[renVARYX]);
        int mid = ifloor(b[renVARYX]);
        int right = ifloor(c[renVARYX]);
        // rasterizing the two parts divided by B
        if (a[renVARYX] != b[renVARYX]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                int top = ifloor(a[renVARYY] + (c[renVARYY] - a[renVARYY]) / (c[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double X[2] = {x0, x1};
                    colorX(X, ren, unif, tex, a, b, c, rgbz);
                }
            }
        }
        if (b[renVARYX] != c[renVARYX]) {
            for (int x0 = mid + 1; x0 <= right; x0++) {
                int bot = iceil(b[renVARYY] + (c[renVARYY] - b[renVARYY]) / (c[renVARYX] - b[renVARYX]) * (x0 - b[renVARYX]));
                int top = ifloor(a[renVARYY] + (c[renVARYY] - a[renVARYY]) / (c[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double X[2] = {x0, x1};
                    colorX(X, ren, unif, tex, a, b, c, rgbz);
                }
            }
        }
    } else {
        // C is in the middle of A and B
        int left = iceil(a[renVARYX]);
        int mid = ifloor(c[renVARYX]);
        int right = ifloor(b[renVARYX]);
        // rasterizing the two parts divided by C
        if (a[renVARYX] != c[renVARYX]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                int top = ifloor(a[renVARYY] + (c[renVARYY] - a[renVARYY]) / (c[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double X[2] = {x0, x1};
                    colorX(X, ren, unif, tex, a, b, c, rgbz);
                }
            }
        }
        if (b[renVARYX] != c[renVARYX]) {
            for (int x0 = mid + 1; x0 <= right; x0++) {
                int bot = iceil(a[renVARYY] + (b[renVARYY] - a[renVARYY]) / (b[renVARYX] - a[renVARYX]) * (x0 - a[renVARYX]));
                int top = ifloor(b[renVARYY] + (c[renVARYY] - b[renVARYY]) / (c[renVARYX] - b[renVARYX]) * (x0 - b[renVARYX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double X[2] = {x0, x1};
                    colorX(X, ren, unif, tex, a, b, c, rgbz);
                }
            }
        }
    }
}


