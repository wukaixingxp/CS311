/**
 * 090mainScene.c
 * Duo Tao and Kaixing Wu
 * Jan 19 2017
 */
#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRN 5
#define renATTRO 6
#define renATTRP 7
#define renATTRDIMBOUND 26
#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 999
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFTHETA 6
#define renUNIFAXX 7
#define renUNIFAXY 8
#define renUNIFAXZ 9
#define renUNIFISOMETRY 10
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#define GLFW_KEY_ENTER 257
#define PI 3.1415926535

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "000pixel.h"
#include "100vector.c"
#include "100matrix.c"
#include "040texture.c"
#include "090renderer.c"

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
    double rot[3][3];
    double axis[3] = {unif[renUNIFAXX], unif[renUNIFAXY], unif[renUNIFAXZ]};
    vecUnit(3, axis, axis);
    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
    mat33AngleAxisRotation(unif[renUNIFTHETA], axis, rot);
    mat44Isometry(rot, trans, (double(*)[4])(&unif[renUNIFISOMETRY]));
    if (unifParent != NULL) {
        mat444Multiply((double(*)[4])(&unifParent[renUNIFISOMETRY]), (double(*)[4])(&unif[renUNIFISOMETRY]), (double(*)[4])(&unif[renUNIFISOMETRY]));
    }
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    double isometry[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++){
            isometry[i][j] = unif[renUNIFISOMETRY + (i * 4) + j];
        }
    }
    double attrXYZ[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    mat441Multiply(isometry, attrXYZ, vary);
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR] * unif[renUNIFR];
    rgb[1] = tex[0]->sample[renTEXG] * unif[renUNIFG];
    rgb[2] = tex[0]->sample[renTEXB] * unif[renUNIFB];
}
#include "100triangle.c"
#include "100mesh.c"
#include "090scene.c"

renRenderer ren = {
    .unifDim = 26, 
    .texDim = 1, 
    .attrDim = 3 + 2 + 3,
    .varyDim = 4,
    .colorPixel = colorPixel,
    .transformVertex = transformVertex,
    .updateUniform = updateUniform
};
// textures
texTexture t1, t2;
texTexture *texRoot[1] = {&t1};
texTexture *texChild[1] = {&t2};
// meshes
meshMesh meshRoot;
meshMesh meshChild;
// nodes
sceneNode root;
sceneNode child;

// for animations
char inc = 1;

// free the memory
void destroyAll() {
    for (char i = 0; i < ren.texDim; i++) {
        texDestroy(texRoot[i]);
    }
    for (char i = 0; i < ren.texDim; i++) {
        texDestroy(texChild[i]);
    }
    meshDestroy(&meshRoot);
    meshDestroy(&meshChild);
    sceneDestroyRecursively(&root);
}

void draw() {
    pixClearRGB(0.0, 0.0, 0.0);
    root.unif[renUNIFTHETA] += 1;
    if (inc) {
        root.unif[renUNIFR] += 0.1;
        root.unif[renUNIFG] += 0.1;
        root.unif[renUNIFB] += 0.1;
        root.unif[renUNIFTRANSX] += 10;
        if (root.unif[renUNIFR] >= 1.0) {
            inc = 0;
        }
    } else {
        root.unif[renUNIFR] -= 0.1;
        root.unif[renUNIFG] -= 0.1;
        root.unif[renUNIFB] -= 0.1;
        root.unif[renUNIFTRANSX] -= 10;
        if (root.unif[renUNIFR] <= 0.0) {
            inc = 1;
        }
    }
    sceneRender(&root, &ren, NULL);
}

void handleTimeStep(double oldTime, double newTime) {
    draw();
}

int main() {
	if (pixInitialize(512, 512, "Rasterizing a Rectangle") != 0) {
		return 1;
	} else { 

		pixClearRGB(0.0, 0.0, 0.0);
        // init textures
        if (texInitializeFile(texRoot[0], "test0.jpg") || texInitializeFile(texChild[0], "test1.jpg")) {
            return 0;
        }
        // init meshes
        meshInitializeBox(&meshRoot, 0, 200, 0, 200, 0, 200);
        meshInitializeBox(&meshChild, 0, 50, 0, 50, 0, 50);
        // meshInitializeSphere(&meshChild, 100, 20, 20);
        // init scenes
        double unifRoot[renATTRDIMBOUND] = {
            1.0, 1.0, 1.0,
            100, 100, 0,
            45,
            1, 1, 1,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };
        double unifChild[renATTRDIMBOUND] = {
            1.0, 0.0, 1.0,
            100, 100, 0,
            45,
            1, 1, 1,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };
        sceneInitialize(&child, &ren, unifRoot, texChild, &meshChild, NULL, NULL);
        sceneInitialize(&root, &ren, unifChild, texRoot, &meshRoot, &child, NULL);
        sceneRender(&root, &ren, NULL);
        pixSetTimeStepHandler(handleTimeStep);
		pixRun();
        destroyAll();
		return 0;
	}
}