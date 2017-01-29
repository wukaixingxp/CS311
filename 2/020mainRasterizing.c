/**
 * 020mainRastering.c
 * Duo Tao
 * Jan 6 2017
 * Main debugging module
 */
#include "020triangle.c"

int main() {
	if (pixInitialize(512, 512, "Rasterizing a Rectangle") != 0) {
		return 1;
	} else {
		pixClearRGB(0.0, 0.0, 0.0);
		// triRender(150, 150, 0, 0, 100, 0, 1, 0, 0);
		// triRender(312, 312, 500, 333, 400, 400, 0, 1, 0);
        // triRender(150.25, 150.15, 250.02, 250.98, 137.50, 213.58, 0, 0, 1);
        // triRender(300, 100, 150, 150, 0, 0, 1, 0, 0);
        // triRender(300, 0, 0, 300, 0, 0, 1, 1, 1);
        triRender(100, 0, 100, 200, 100, 300, 1, 1, 1);
		pixRun();
		return 0;
	}
}
