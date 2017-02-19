/**
 * 170mainSpecular.c
 * Kaixing Wu
 * Feb 8 2017
 */
#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRN 5
#define renATTRO 6
#define renATTRP 7
#define renATTRDIMBOUND 50

#define renVARYX 0
#define renVARYY 1
#define renVARYZ 2
#define renVARYW 3
#define renVARYS 4
#define renVARYT 5
#define renVARYWX 6
#define renVARYWY 7
#define renVARYWZ 8
#define renVARYN 9
#define renVARYO 10
#define renVARYP 11

#define renVARYDIMBOUND 12
#define renVERTNUMBOUND 4000

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
#define renUNIFLX 10
#define renUNIFLY 11
#define renUNIFLZ 12
#define renUNIFLR 13
#define renUNIFLG 14
#define renUNIFLB 15
#define renUNIFCX 16
#define renUNIFCY 17
#define renUNIFCZ 18
#define renUNIFISOMETRY 19
#define renUNIFCamera 35
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#define GLFW_KEY_ENTER 257
#define W 87
#define S 83
#define A 65
#define D 68
#define PI 3.1415926535

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <float.h>
#include "000pixel.h"
#include "100vector.c"
#include "130matrix.c"
#include "040texture.c"
#include "110depth.c"
#include "130renderer.c"
double min0(double a){
    if (a < 0){
        return 0.0;
    }
    return a;
}
//Camera setting
double target[3] = {250.0, 200.0, 0.0};
double cameraRho = 250.0, cameraPhi = M_PI / 3.0, cameraTheta = M_PI / 3.0;
double specularColor[3] = {1.0, 1.0, 1.0};
// Light setting
double lightPos[6] = {100, 100, 0, 1.0, 1.0, 1.0}; 
double shiness = 5;
// Set the Camera in Unif
void setCamera(double unif[], double cameraPos[3]) {
    unif[renUNIFCX] = cameraPos[0];
    unif[renUNIFCY] = cameraPos[1];
    unif[renUNIFCZ] = cameraPos[2];
}
// Set the Lighting in Unif
void setLight(double unif[], double lightPos[]) {
    unif[renUNIFLX] = lightPos[0];
    unif[renUNIFLY] = lightPos[1];
    unif[renUNIFLZ] = lightPos[2];
    unif[renUNIFLR] = lightPos[3];
    unif[renUNIFLG] = lightPos[4];
    unif[renUNIFLB] = lightPos[5];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    // Set Carmera matrix to ren->viewing
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            unif[renUNIFCamera + i * 4 + j] = ren->viewing[i][j];
        }
    }
    // Set the light position and RGB
    setLight(unif, lightPos);
    // Calculate and set the Camera position.
    double CameraX = cameraRho * sin(cameraPhi) * cos(cameraTheta);
    double CameraY = cameraRho * cos(cameraPhi) * sin(cameraTheta);
    double CameraZ = cameraRho * cos(cameraPhi);
    double cameraPos[3] = {CameraX, CameraY, CameraZ};
    setCamera(unif, cameraPos);
    // Update the uniform matrix
    double rot[3][3];
    double axis[3] = {unif[renUNIFAXX], unif[renUNIFAXY], unif[renUNIFAXZ]};
    vecUnit(3, axis, axis);
    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
    mat33AngleAxisRotation(unif[renUNIFTHETA], axis, rot);
    mat44Isometry(rot, trans, (double(*)[4])(&unif[renUNIFISOMETRY]));
    double unifCopy[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            unifCopy[i][j] = unif[renUNIFISOMETRY + i * 4 + j];
        }
    }
    if (unifParent != NULL) {
        mat444Multiply((double(*)[4])(&unifParent[renUNIFISOMETRY]), unifCopy, (double(*)[4])(&unif[renUNIFISOMETRY]));
    }
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    double isometry[4][4];
    double N[4];
    double attrXYZ[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    double NOP0[4] = {attr[renATTRN], attr[renATTRO], attr[renATTRP], 0};
    double NOP[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), NOP0, NOP);
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), attrXYZ, N);
    mat441Multiply((double(*)[4])(&unif[renUNIFCamera]), N, vary);
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
    vary[renVARYWX] = attr[renATTRX];
    vary[renVARYWY] = attr[renATTRY];
    vary[renVARYWZ] = attr[renATTRZ];
    vary[renVARYN] = NOP[0];
    vary[renVARYO] = NOP[1];
    vary[renVARYP] = NOP[2];
}


/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], double vary[], double rgbz[]) {
    // Calculate the dot product of Light and NOP
    double light[3];
    double lightUnit[3];
    double NOP[3] = {vary[renVARYN], vary[renVARYO], vary[renVARYP]};
    double NOPUnit[3];
    light[0] = unif[renUNIFLX] - vary[renVARYWX]; 
    light[1] = unif[renUNIFLY] - vary[renVARYWY]; 
    light[2] = unif[renUNIFLZ] - vary[renVARYWZ];
    vecUnit(3, light, lightUnit);
    vecUnit(3, NOP, NOPUnit);
    double dIntensity = min0(vecDot(3, lightUnit, NOPUnit));

    // Calcuate the reflection light vector and Camera vector
    double reflection[3];
    double reflectionUnit[3];
    double P2[3];
    double CameraV[3];
    double CameraUnit[3];
    vecScale(3, 2*(dIntensity), NOP, P2);
    vecSubtract(3, P2, lightUnit, reflection);
    CameraV[0] = unif[renUNIFCX] - vary[renVARYWX]; 
    CameraV[1] = unif[renUNIFCY] - vary[renVARYWY]; 
    CameraV[2] = unif[renUNIFCZ] - vary[renVARYWZ];
    vecUnit(3, reflection, reflectionUnit);
    vecUnit(3, CameraV, CameraUnit);
    double rIntensity = min0(vecDot(3, reflectionUnit, CameraUnit));
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgbz[0] = tex[0]->sample[renTEXR] * unif[renUNIFR] * unif[renUNIFLR] * dIntensity * specularColor[0] * (pow(rIntensity,shiness));
    rgbz[1] = tex[0]->sample[renTEXG] * unif[renUNIFG] * unif[renUNIFLG] * dIntensity * specularColor[1] * (pow(rIntensity,shiness));
    rgbz[2] = tex[0]->sample[renTEXB] * unif[renUNIFB] * unif[renUNIFLB] * dIntensity * specularColor[2]* (pow(rIntensity,shiness)); 
    rgbz[3] = vary[renVARYZ];
}

#include "110triangle.c"
#include "140clipping.c"
#include "140mesh.c"
#include "090scene.c"

// textures
texTexture t1, t2, t3;
texTexture *texRoot[1] = {&t1};
texTexture *texChild[1] = {&t2};
texTexture *texGrandChild[1] = {&t3};

// meshes
meshMesh meshRoot;
meshMesh meshChild;
meshMesh meshGrandChild;

// nodes
sceneNode root;
sceneNode child;
sceneNode grandChild;

// depth buffer
depthBuffer depth;

// init renderer
renRenderer ren = {
    .unifDim = 52, 
    .texDim = 1, 
    .attrDim = 3 + 2 + 3,
    .varyDim = 4 + 2+ 3 + 3,
    .colorPixel = colorPixel,
    .transformVertex = transformVertex,
    .updateUniform = updateUniform,
    .depth = &depth,
    .cameraRotation = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    },
    .viewing = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    },
    .cameraTranslation = {0, 0, 0},
    .projection = {-26.794092, 26.794092, -26.794092, 26.794092, -100.000000, -1.000000},
    .projectionType = 0,
    .viewport = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    },
};

// free the memory for this node
void destroyAllForNode(sceneNode *node) {
    for (char i = 0; i < ren.texDim; i++) {
        texDestroy(node->tex[i]);
    }
    meshDestroy(node->mesh);
}

void draw() {
    pixClearRGB(0.0, 0.0, 0.0);
    depthClearZs(&depth, -DBL_MAX);
    renLookAt(&ren, target, cameraRho, cameraPhi, cameraTheta);
    renUpdateViewing(&ren);
    sceneRender(&root, &ren, NULL);
}

void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {
    if (key == GLFW_KEY_L) {
        if (ren.projectionType == renPERSPECTIVE)
            renSetFrustum(&ren, renORTHOGRAPHIC, M_PI / 6.0, cameraRho, 10.0);
        else
            renSetFrustum(&ren, renPERSPECTIVE, M_PI / 6.0, cameraRho, 10.0);
    } else if (key == GLFW_KEY_LEFT){
        target[0] -= 10.0;
    }
    else if (key == GLFW_KEY_RIGHT){
        target[0] += 10.0;
    }
    else if (key == GLFW_KEY_DOWN)
        target[1] -= 10.0;
    else if (key == GLFW_KEY_UP)
        target[1] += 10.0;
    else if (key == GLFW_KEY_O)
        cameraTheta -= 0.1;
    else if (key == GLFW_KEY_P)
        cameraTheta += 0.1;
    else if (key == GLFW_KEY_I)
        cameraPhi -= 0.1;
    else if (key == GLFW_KEY_K)
        cameraPhi += 0.1;
    else if (key == GLFW_KEY_U)
        cameraRho -= 1.0;
    else if (key == GLFW_KEY_J)
        cameraRho += 1.0;
    draw();
}



void handleTimeStep(double oldTime, double newTime) {
    renUpdateViewing(&ren);
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
    draw();
}
int main() {
    if (pixInitialize(512, 512, "Rasterizing a Rectangle") != 0) {
        return 1;
    } else { 
        // init textures
        if (texInitializeFile(texRoot[0], "testRoot.jpg") 
                || texInitializeFile(texChild[0], "testChild.jpg")
                || texInitializeFile(texGrandChild[0], "testGrandChild.jpg")) {
            return 0;
        }

        // init meshes
        // meshInitializeBox(&meshRoot, 0, 200, 0, 200, 0, 200);
        meshInitializeSphere(&meshRoot, 50, 40, 40);
        meshInitializeSphere(&meshChild, 50, 40, 40);
        meshInitializeSphere(&meshGrandChild, 25, 40, 40);
        // init depth buffer
        depthInitialize(&depth, 512, 512);
        // inti ren->projection
        renLookAt(&ren, target, cameraRho, cameraPhi, cameraTheta);
        renSetFrustum(&ren, renORTHOGRAPHIC, M_PI / 6.0, cameraRho, 10.0);
        // init unif
        double unifRoot[renATTRDIMBOUND] = {
            1.0, 0.0, 0.0,
            250, 250, 0,
            0,
            1, -1, 0,
            300, 300, 1,
            1.0, 1.0, 1.0,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };
        double unifChild[renATTRDIMBOUND] = {
            0.0, 0.0, 1.0,
            200, 200, 0,
            0,
            1, -1, 0,
            300, 300, 1,
            1.0, 1.0, 1.0,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };
        double unifGrandChild[renATTRDIMBOUND] = {
            0.0, 0.0, 0.0,
            50, 50, 0,
            0,
            1, -1, 0,
            300, 300, 1,
            1.0, 1.0, 1.0,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0, 
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };
        // init scenes
        sceneInitialize(&grandChild, &ren, unifGrandChild, texGrandChild, &meshGrandChild, NULL, NULL);
        sceneInitialize(&child, &ren, unifChild, texChild, &meshChild, &grandChild, NULL);
        sceneInitialize(&root, &ren, unifRoot, texRoot, &meshRoot, NULL, NULL);

        // run it
        pixSetKeyUpHandler(handleKeyUp);
        pixSetTimeStepHandler(handleTimeStep);
        pixRun();

        // destroy all stuff
        destroyAllForNode(&root);
        destroyAllForNode(&child);
        destroyAllForNode(&grandChild);
        sceneDestroyRecursively(&root);
        depthDestroy(ren.depth);
        return 0;
    }
}