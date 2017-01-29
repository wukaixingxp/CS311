/**
 * 040MainTexturing.c
 * Duo Tao and Kaixing Wu
 * Jan 11 2017
 */
#include "000pixel.h"
#include "030vector.c"
#include "030matrix.c"
#include "040texture.c"
#include "040triangle.c"

#define GLFW_KEY_ENTER 257
texTexture tex;
double gA[2] = {100, 100};
double gB[2] = {200, 200};
double gC[2] = {110, 200};
double gRGB[3] = {1, 1, 1};
double gAlpha[2] = {0, 0};
double gBeta[2] = {1, 1};
double gGamma[2] = {0, 1};


// switch filtering with ENTER
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {
    if (!shiftIsDown && !controlIsDown && !altOptionIsDown && !superCommandIsDown && key == GLFW_KEY_ENTER) {
        // rasterization again
        texSetFiltering(&tex,!tex.filtering);
        pixClearRGB(0.0, 0.0, 0.0);
        triRender(gA, gB, gC, gRGB, &tex, gAlpha, gBeta, gGamma);
    }
}

int main() {
	if (pixInitialize(512, 512, "Rasterizing a Rectangle") != 0) {
		return 1;
	} else {
		pixClearRGB(0.0, 0.0, 0.0);
        if (texInitializeFile(&tex, "test.jpg") == 0) {
            triRender(gA, gB, gC, gRGB, &tex, gAlpha, gBeta, gGamma);
            pixSetKeyUpHandler(handleKeyUp);
    		pixRun();
            texDestroy(&tex);
        }
		return 0;
	}
}
