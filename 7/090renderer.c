/**
 * 090renderer.c
 * Duo Tao and Kaixing Wu
 * Jan 19 2017
 */

typedef struct renRenderer renRenderer;

struct renRenderer {
	int unifDim;		
	int texDim;			
	int attrDim;
	int varyDim;
    void (*colorPixel)(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]);
    void (*transformVertex)(renRenderer *ren, double unif[], double attr[], double vary[]);
    void (*updateUniform) (renRenderer *ren, double unif[], double unifParent[]);			
};


