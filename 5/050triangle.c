/**
 * 050triangle.c
 * Duo Tao and Kaixing Wu
 * Jan 15 2017
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
    double attr[ren->attrDim];
    attr[renATTRX] = x[0];
    attr[renATTRY] = x[1];

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
    double alphaT[2] = {a[renATTRS], a[renATTRT]};
    double betaT[2] = {b[renATTRS], b[renATTRT]};
    double gammaT[2] = {c[renATTRS], c[renATTRT]};
    double abTex[2];
    double acTex[2];
    vecSubtract(2, betaT, alphaT, abTex);
    vecSubtract(2, gammaT, alphaT, acTex);
    vecScale(2, pq[0], abTex, abTex);
    vecScale(2, pq[1], acTex, acTex);
    vecAdd(2, abTex, acTex, st);
    vecAdd(2, st, alphaT, st);
    attr[renATTRS] = st[0];
    attr[renATTRT] = st[1];

    // get the interpolated RGB at x
    double rgbC[3]; 
    double abColor[3];
    double acColor[3];
    double alphaC[3] = {a[renATTRR], a[renATTRG], a[renATTRB]};
    double betaC[3] = {b[renATTRR], b[renATTRG], b[renATTRB]};
    double gammaC[3] = {c[renATTRR], c[renATTRG], c[renATTRB]};
    vecSubtract(3, betaC, alphaC, abColor);
    vecSubtract(3, gammaC, alphaC, acColor);
    vecScale(3, pq[0], abColor, abColor);
    vecScale(3, pq[1], acColor, acColor);
    vecAdd(3, abColor, acColor, rgbC);
    vecAdd(3, rgbC, alphaC, rgbC);
    attr[renATTRR] = rgbC[0];
    attr[renATTRG] = rgbC[1];
    attr[renATTRB] = rgbC[2];

    // figure out the color with these and save it into rgb
    colorPixel(ren, unif, tex, attr, rgb); 
}

// rotate A to the left
void vertexPreprocess(renRenderer *ren, double a[], double b[], double c[]) {
    double temp[ren->attrDim];
    if (b[renATTRX] < a[renATTRX] && b[renATTRX] <= c[renATTRX]) {
        vecCopy(ren->attrDim, a, temp);
        vecCopy(ren->attrDim, b, a);
        vecCopy(ren->attrDim, c, b);
        vecCopy(ren->attrDim, temp, c);
    } else if (c[renATTRX] < a[renATTRX] && c[renATTRX] <= b[renATTRX]) {
        vecCopy(ren->attrDim, a, temp);
        vecCopy(ren->attrDim, c, a);
        vecCopy(ren->attrDim, b, c);
        vecCopy(ren->attrDim, temp, b);
    }
}

void triRender(renRenderer *ren, double unif[], texTexture *tex[], double a[], double b[], double c[]) {
    double rgb[3]; // place to save the color at all positions x

    vertexPreprocess(ren, a, b, c);

    // handling two remaining cases
    if (b[renATTRX] < c[renATTRX]) {
        // B is in the middle of A and C
        int left = iceil(a[renATTRX]);
        int mid = ifloor(b[renATTRX]);
        int right = ifloor(c[renATTRX]);
        // rasterizing the two parts divided by B
        if (a[0] != b[0]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[renATTRY] + (b[renATTRY] - a[renATTRY]) / (b[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
                int top = ifloor(a[renATTRY] + (c[renATTRY] - a[renATTRY]) / (c[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
                for (int x1 = bot; x1 <= top; x1++) {
                    double x[2] = {x0, x1};
                    getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(a[renATTRY] + (b[renATTRY] - a[renATTRY]) / (b[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
            int top = ifloor(a[renATTRY] + (c[renATTRY] - a[renATTRY]) / (c[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
            for (int x1 = bot; x1 <= top; x1++) {
                double x[2] = {x0, x1};
                getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
            }
        }
    } else {
        // C is in the middle of A and B
        int left = iceil(a[renATTRX]);
        int mid = ifloor(c[renATTRX]);
        int right = ifloor(b[renATTRX]);
        // rasterizing the two parts divided by C
        if (a[0] != c[0]) {
            for (int x0 = left; x0 <= mid; x0++) {
                int bot = iceil(a[renATTRY] + (b[renATTRY] - a[renATTRY]) / (b[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
                int top = ifloor(a[renATTRY] + (c[renATTRY] - a[renATTRY]) / (c[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
                printf("%d\n", top);
                for (int x1 = bot; x1 <= top; x1++) {
                    double x[2] = {x0, x1};
                    getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                }
            }
        }
        for (int x0 = mid + 1; x0 <= right; x0++) {
            int bot = iceil(a[renATTRY] + (b[renATTRY] - a[renATTRY]) / (b[renATTRX] - a[renATTRX]) * (x0 - a[renATTRX]));
            int top = iceil(b[renATTRY] + (c[renATTRY] - b[renATTRY]) / (c[renATTRX] - b[renATTRX]) * (x0 - b[renATTRX]));
            for (int x1 = bot; x1 <= top; x1++) {
                double x[2] = {x0, x1};
                getColorAtX(x, ren, unif, tex, a, b, c, rgb);  
                pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
            }
        }
    }
}


