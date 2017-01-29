/**
 * 070mainMesh.c
 * Duo Tao and Kaixing Wu
 * Jan 16 2017
 */
#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2
#define GLFW_KEY_ENTER 257
#define renATTRDIMBOUND 16
#define renVARYDIMBOUND 16
#include <stdio.h>
#include <stdarg.h>
#include "080renderer.c"
#include "000pixel.h"
#include "070vector.c"
#include "030matrix.c"
#include "040texture.c"

double min(double a, double b) {
    return a < b ? a : b;
}
/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double attr[], double rgb[]) {
    texSample(tex[0], attr[renATTRS], attr[renATTRT]);
    texSample(tex[1], attr[renATTRS], attr[renATTRT]);
    rgb[0] = min(tex[0]->sample[renTEXR], tex[1]->sample[renTEXR]) * unif[renUNIFR] * attr[renATTRR];
    rgb[1] = min(tex[0]->sample[renTEXG], tex[1]->sample[renTEXR]) * unif[renUNIFG] * attr[renATTRG];
    rgb[2] = min(tex[0]->sample[renTEXB], tex[1]->sample[renTEXR]) * unif[renUNIFB] * attr[renATTRB];
}

#include "080triangle.c"
#include "070mesh.c"

renRenderer ren = {
    .unifDim = 3, 
    .texDim = 2, 
    .attrDim = 7,
    .varyDim = 7
};

int tri[6] = {0, 1, 2, 0, 3, 1};
double vert[28] =  {
    100, 100, 0, 0, 1.0, 1.0, 1.0,
    200, 200, 1, 1, 1.0, 1.0, 1.0,
    110, 200, 0, 1, 1.0, 1.0, 1.0,
    200, 0, 1, 0, 1.0, 1.0, 1.0
};
meshMesh mesh = {
    .triNum = 2,
    .vertNum = 4,
    .attrDim = 7,
    .tri = tri,                
    .vert = vert
};
double gUnif[renATTRDIMBOUND] = {
    1.0, 1.0, 1.0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

texTexture t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16;
texTexture *tex[renATTRDIMBOUND] = {
    &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10, &t11, &t12, &t13, &t14, &t15, &t16
};

// switch filtering with ENTER
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {
    if (!shiftIsDown && !controlIsDown && !altOptionIsDown && !superCommandIsDown && key == GLFW_KEY_ENTER) {
        // rasterization again
        // texSetFiltering(tex[0], !(tex[0])->filtering);
        // pixClearRGB(0.0, 0.0, 0.0);
        // triRender(&ren, gUnif, tex, gA, gB, gC);
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
        // triRender(&ren, gUnif, tex, gA, gB, gC);
        // pixSetKeyUpHandler(handleKeyUp);
        meshRender(&mesh, &ren, gUnif, tex);
		pixRun();
        for (char i = 0; i < ren.texDim; i++) {
            texDestroy(tex[i]);
        }
		return 0;
	}
}