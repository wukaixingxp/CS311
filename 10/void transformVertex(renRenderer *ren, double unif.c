void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    /*Then perform the viewport transformation. */
    /* First, copy attr S and T to vary. */
    /* Then, do the modeling and viewing transformation. */
    double toBeTransformed[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    double transformed1[4], transformed2[4], transformed3[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), toBeTransformed, transformed1);
    mat441Multiply((double(*)[4])(&unif[renUNIFCamera]), transformed1, transformed2); 
    
    printf("Transform2 is: {%f, %f, %f, %f}\n", transformed2[0], transformed2[1], transformed2[2], transformed2[3]);
    vecScale(4, 1/transformed2[3], transformed2, transformed2);
    mat441Multiply(ren->viewport, transformed2, transformed3);
    //printf("Viewport is: {%f, %f, %f, %f}\n", ren->viewport[0][0], ren->viewport[0][1], ren->viewport[0][2], ren->viewport[0][3]);
    vary[renVARYX] = transformed3[0];
    vary[renVARYY] = transformed3[1];
    vary[renVARYZ] = transformed3[2];
    vary[3] = transformed3[3];
    //printf("Vary is: {%f, %f, %f, %f}\n", vary[0], vary[1], vary[2], vary[3]);
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
    /* Assign the transformed screen coordinates to vary. */
    // vary[renVARYX] = transformed2[0];
    // vary[renVARYY] = transformed2[1];
    // vary[renVARYZ] = transformed2[2];
    // vary[renVARYW] = transformed2[3];
    //printf("Vary is: {%f, %f, %f, %f}\n", vary[0], vary[1], vary[2], vary[3]);
}