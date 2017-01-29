/**
 * 090mainScene.c
 * Duo Tao and Kaixing Wu
 * Jan 19 2017
 */
#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6
#define renATTRDIMBOUND 16
#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6
#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 99
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTHETA 3
#define renUNIFTRANSX 4
#define renUNIFTRANSY 5
#define renUNIFISOMETRY 6
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#define GLFW_KEY_ENTER 257
#define PI 3.1415926535

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "000pixel.h"
#include "070vector.c"
#include "090matrix.c"
#include "040texture.c"
#include "090renderer.c"

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    texSample(tex[1], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR] * unif[renUNIFR] * vary[renVARYR];
    rgb[1] = tex[0]->sample[renTEXG] * unif[renUNIFG] * vary[renVARYG];
    rgb[2] = tex[0]->sample[renTEXB] * unif[renUNIFB] * vary[renVARYB];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    if (unifParent == NULL)
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX], 
            unif[renUNIFTRANSY], (double(*)[3])(&unif[renUNIFISOMETRY]));
    else {
        double m[3][3];
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX], 
            unif[renUNIFTRANSY], m);
        mat333Multiply((double(*)[3])(&unifParent[renUNIFISOMETRY]), m, 
            (double(*)[3])(&unif[renUNIFISOMETRY]));
    }
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    double isometry[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            isometry[i][j] = unif[renUNIFISOMETRY + (i * 3) + j];
        }
    }
    double attrXY[3] = {attr[renATTRX], attr[renATTRY], 1};
    mat331Multiply(isometry, attrXY, vary);
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
    vary[renVARYR] = attr[renATTRR];
    vary[renVARYG] = attr[renATTRG];
    vary[renVARYB] = attr[renATTRB];
}
#include "090triangle.c"
#include "090mesh.c"
#include "090scene.c"


renRenderer ren = {
    .unifDim = 15, 
    .texDim = 2, 
    .attrDim = 7,
    .varyDim = 7,
    .colorPixel = colorPixel,
    .transformVertex = transformVertex,
    .updateUniform = updateUniform
};
meshMesh mesh;
texTexture t1, t2;
texTexture *tex[2] = {&t1, &t2};
sceneNode grandChild1;
sceneNode child2;
sceneNode child1;
sceneNode root;

// free the memory
void destroyAll() {
    for (char i = 0; i < ren.texDim; i++) {
        texDestroy(tex[i]);
    }
    meshDestroy(&mesh);
    sceneDestroyRecursively(&root);
}

// init mesh
void getMesh(meshMesh *mesh, int triNum, int vertNum, int attrDim, double vert[28], int tri[6]) {
    // meshInitialize(meshMesh *mesh, int triNum, int vertNum, int attrDim)
    meshInitialize(mesh, triNum, vertNum, attrDim);
    // meshSetVertex(meshMesh *mesh, int vert, double attr[])
    for (int i = 0; i < mesh->vertNum; i++) {
        meshSetVertex(mesh, i, &vert[i * ren.varyDim]);
    }
    // meshSetTriangle(meshMesh *mesh, int tri, int i, int j, int k)
    for (int i = 0; i < mesh->triNum; i++) {
        meshSetTriangle(mesh, i, tri[i * 3], tri[i * 3 + 1], tri[i * 3 + 2]);
    }
}

int main() {
	if (pixInitialize(512, 512, "Rasterizing a Rectangle") != 0) {
		return 1;
	} else {
		pixClearRGB(0.0, 0.0, 0.0);
        for (char i = 0; i < ren.texDim; i++) {
            char pic[20];
            sprintf(pic, "test%d.jpg", i);
            if (texInitializeFile(tex[i], pic) != 0) {
                return 0;
            }
        }

        // init mesh
        int tri[6] = {0, 1, 2, 0, 2, 3};
        double vert[28] =  {
            0, 0, 0, 0, 1.0, 1.0, 1.0,
            200, 0, 1, 1, 1.0, 1.0, 1.0,
            200, 100, 0, 1, 1.0, 1.0, 1.0,
            200, 200, 1, 0, 1.0, 1.0, 1.0
        };
        getMesh(&mesh, 2, 4, 7, vert, tri);

        // init scenes
        double unifG1[renATTRDIMBOUND] = {
            1.0, 1.0, 1.0,
            45, 0, 0,
            0, 0, 0, 
            0, 0, 0, 
            0, 0, 0, 0
        };
        sceneInitialize(&grandChild1, &ren, unifG1, tex, &mesh, NULL, NULL);
        double unifC2[renATTRDIMBOUND] = {
            1.0, 1.0, 1.0,
            45, 0, 0,
            0, 0, 0, 
            0, 0, 0, 
            0, 0, 0, 0
        };
        sceneInitialize(&child2, &ren, unifC2, tex, &mesh, NULL, NULL);
        double unifC1[renATTRDIMBOUND] = {
            1.0, 1.0, 1.0,
            90, 0, 0,
            0, 0, 0, 
            0, 0, 0, 
            0, 0, 0, 0
        };
        sceneInitialize(&child1, &ren, unifC1, tex, &mesh, &grandChild1, &child2);
        double unifR[renATTRDIMBOUND] = {
            1.0, 1.0, 1.0,
            45, 300, 200,
            0, 0, 0, 
            0, 0, 0, 
            0, 0, 0, 0
        };
        sceneInitialize(&root, &ren, unifR, tex, &mesh, &child1, NULL);

        sceneRender(&root, &ren, NULL);
		pixRun();

        destroyAll();
		return 0;
	}
}