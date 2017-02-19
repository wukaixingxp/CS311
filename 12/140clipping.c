#define aCliped 0
#define bCliped 1
#define cCliped 2

void transV(renRenderer *ren, double unif[], texTexture *tex[], double aVary[], double bVary[], 
	double cVary[]) {
	// This function performs the homogeneous division and viewport matrix
	double a[4]= {0, 0, 0, 0};
	double b[4]= {0, 0, 0, 0};
	double c[4]= {0, 0, 0, 0};
    vecCopy(4, aVary,a);
    vecCopy(4, bVary,b);
    vecCopy(4, cVary,c);
	double aView[renVARYDIMBOUND];
	double bView[renVARYDIMBOUND];
	double cView[renVARYDIMBOUND];
	
    vecScale(4, 1/aVary[3],a, a);
    vecScale(4, 1/bVary[3],b, b);
    vecScale(4, 1/cVary[3],c, c);
    vecCopy(renVARYDIMBOUND, aVary, aView);
    vecCopy(renVARYDIMBOUND, bVary, bView);
    vecCopy(renVARYDIMBOUND, cVary, cView);
    mat441Multiply(ren->viewport, a, aView);
    mat441Multiply(ren->viewport, b, bView);
    mat441Multiply(ren->viewport, c, cView);
    //printf("%f  %f    %f \n", aView[renVARYN], aView[renVARYO], aView[renVARYP]);
    triRender(ren, unif, tex, aView, bView, cView); 
  }
// this function checks whether a vertex is clipped or not
int isCliped(double vertex[]){
    if (vertex[renVARYW] < 0){
        return 1;
    }
    if (vertex[renVARYZ] > vertex[renVARYW]){
        return 1;
    }
    return 0;
}
// this function find the cliped vertex given two vertex V1, V2, where V1 is clipped, V2 is not.
void findT(renRenderer *ren, double V1[], double V2[], double T[]) {
    double t = (V1[renVARYZ] - V1[renVARYY]) / (V1[renVARYZ] - V1[renVARYY] + V2[renVARYY] - V2[renVARYZ]);
    double V2minusV1[ren->varyDim];
    vecSubtract(ren->varyDim, V2, V1, V2minusV1);
    double TtimesV2V1[ren->varyDim];
    vecScale(ren->varyDim, t, V2minusV1, TtimesV2V1);
    vecAdd(ren->varyDim, V1, TtimesV2V1, T);
}
// This function check how many vertex in a given triangle is cliped and store their cliped vertex into a list.
void checkTriangle(renRenderer *ren, double aVary[], double bVary[], double cVary[], int clipedVertex[3]){
    clipedVertex[aCliped] = isCliped(aVary);
    clipedVertex[bCliped] = isCliped(bVary);
    clipedVertex[cCliped] = isCliped(cVary);
}
// This function tests how many vertex in the given triangle should be cliped, and clips the triangle accordingly.
void clipRender(renRenderer *ren, double unif[], texTexture *tex[], double aVary[], double bVary[], 
    double cVary[]) {
    int clipedVertex[3];
    double TinAC[ren->varyDim];
    double TinBC[ren->varyDim];
    double TinAB[ren->varyDim];
    checkTriangle(ren, aVary, bVary, cVary, clipedVertex);
    int numCliped = 0;
    // use the sum of numCliped to find out which case the triangle is
    for (int i = 0; i < 3; i++){
        numCliped += clipedVertex[i];
    }
    switch (numCliped) {
        case 0:
            transV(ren, unif, tex, aVary, bVary, cVary);
            break;
        case 1:
            if (clipedVertex[aCliped]) {
                findT(ren, aVary, bVary, TinAB);
                transV(ren, unif, tex, TinAB, bVary, cVary);
                findT(ren, aVary, cVary, TinAC);
                transV(ren, unif, tex, TinAB, TinAC, bVary);
            } else if (clipedVertex[bCliped]) {
                findT(ren, bVary, aVary, TinAB);
                transV(ren, unif, tex, TinAB, aVary, cVary);
                findT(ren, bVary, cVary, TinBC);
                transV(ren, unif, tex, TinAB, TinBC, cVary);
            } else if (clipedVertex[cCliped]) {
                findT(ren, cVary, aVary, TinAC);
                transV(ren, unif, tex, TinAB, bVary, aVary);
                findT(ren, cVary, aVary, TinBC);
                transV(ren, unif, tex, TinAC, TinBC, bVary);
            }
            break;
        case 2:
            if (!clipedVertex[aCliped]) {
                findT(ren, bVary, aVary, TinAB);
                findT(ren, cVary, aVary, TinAC);
                transV(ren, unif, tex, TinAB, TinAC, aVary);
            } else if (!clipedVertex[bCliped]) {
                findT(ren, aVary, bVary, TinAB);
                findT(ren, cVary, bVary, TinBC);
                transV(ren, unif, tex, TinAB, bVary, TinBC);
            } else if (!clipedVertex[cCliped]) {
                findT(ren, aVary, cVary, TinAC);
                findT(ren, bVary, cVary, TinBC);
                transV(ren, unif, tex, TinAB, TinBC, cVary);
            }
            break;
        case 3:
            break;
    }
}

