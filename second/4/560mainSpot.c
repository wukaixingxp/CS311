


/* On macOS, compile with...
    clang 560mainSpot.c -lglfw -framework OpenGL
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "500shader.c"
#include "530vector.c"
#include "510mesh.c"
#include "520matrix.c"
#include "520camera.c"
#include "540texture.c"
#include "540scene.c"
#include "560light.c"
/* create all the locations and programs. */
GLuint program;
GLint attrLocs[3];
GLint viewingLoc, modelingLoc;
GLint unifLocs[1];
GLint textureLoc[1];
GLint lightLocs[5];
GLint camLoc;
/* create all the structs.*/
camCamera cam;
lightLight light;
texTexture texChild;
texTexture texRoot;
texTexture texSibling;
texTexture *texChildList[1] = {&texChild};
texTexture *texRootList[1] = {&texRoot};
texTexture *texSiblingList[1] = {&texSibling};
/* create all the global attributes. */
GLdouble alpha = 0.0;
GLdouble lightTranslation[3] = {100, 100, 200};
GLdouble lightColor[3] = {1.0, 1.0, 1.0};
GLdouble lightAtten[3] = {1.0, 0.0, 0.0};
GLdouble lightAngle = M_PI/6.0;

/* Allocate three meshes and three scene graph nodes. */
meshGLMesh rootMesh, childMesh, siblingMesh;
sceneNode rootNode, childNode, siblingNode;

void handleError(int error, const char *description) {
	fprintf(stderr, "handleError: %d\n%s\n", error, description);
}

void handleResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
	camSetWidthHeight(&cam, width, height);
}

void handleKey(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
	int shiftIsDown = mods & GLFW_MOD_SHIFT;
	int controlIsDown = mods & GLFW_MOD_CONTROL;
	int altOptionIsDown = mods & GLFW_MOD_ALT;
	int superCommandIsDown = mods & GLFW_MOD_SUPER;
	if (action == GLFW_PRESS && key == GLFW_KEY_L) {
		camSwitchProjectionType(&cam);
	} else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_O)
			camAddTheta(&cam, -0.1);
		else if (key == GLFW_KEY_P)
			camAddTheta(&cam, 0.1);
		else if (key == GLFW_KEY_I)
			camAddPhi(&cam, -0.1);
		else if (key == GLFW_KEY_K)
			camAddPhi(&cam, 0.1);
		else if (key == GLFW_KEY_U)
			camAddDistance(&cam, -0.1);
		else if (key == GLFW_KEY_J)
			camAddDistance(&cam, 0.1);
	}
}

/* Returns 0 on success, non-zero on failure. Warning: If initialization fails 
midway through, then does not properly deallocate all resources. But that's 
okay, because the program terminates almost immediately after this function 
returns. */
int initializeScene(void) {
	// Initialize texture.
	glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    if (texInitializeFile(&texChild, "testChild.jpg", GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP) != 0) {
        glfwTerminate();
        return 3;
       }
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    if (texInitializeFile(&texSibling, "testSibling.jpg", GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP) != 0) {
        glfwTerminate();
        return 3;
    }
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    if (texInitializeFile(&texRoot, "testRoot.jpg", GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP) != 0) {
        glfwTerminate();
        return 3;
    }
	/* Initialize meshes. */
	meshMesh mesh;
	if (meshInitializeCapsule(&mesh, 0.5, 2.0, 16, 32) != 0)
		return 1;
	meshGLInitialize(&rootMesh, &mesh);
	meshDestroy(&mesh);
	if (meshInitializeBox(&mesh, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != 0)
		return 2;
	meshGLInitialize(&childMesh, &mesh);
	meshDestroy(&mesh);
	if (meshInitializeSphere(&mesh, 0.5, 16, 32) != 0)
		return 3;
	meshGLInitialize(&siblingMesh, &mesh);
	meshDestroy(&mesh);
	/* Initialize scene graph nodes. */
	if (sceneInitialize(&siblingNode, 2, &siblingMesh, NULL, NULL,1) != 0)
		return 4;
	if (sceneInitialize(&childNode, 2, &childMesh, NULL, NULL,1) != 0)
		return 5;
	if (sceneInitialize(&rootNode, 2, &rootMesh, &childNode, &siblingNode,1) != 0)
		return 6;
	/* Customize the uniforms. */
	GLdouble trans[3] = {1.0, 0.0, 0.0};
	sceneSetTranslation(&childNode, trans);
	vecSet(3, trans, 0.0, 1.0, 0.0);
	sceneSetTranslation(&siblingNode, trans);
	GLdouble unif[1] = {1.0};
	sceneSetUniform(&siblingNode, unif);
	sceneSetUniform(&childNode, unif);
	sceneSetUniform(&rootNode, unif);
	sceneSetTexture(&rootNode, texRootList);	
	sceneSetTexture(&childNode, texChildList);
	sceneSetTexture(&siblingNode, texSiblingList);
	return 0;
}

void destroyScene(void) {
	meshGLDestroy(&siblingMesh);
	meshGLDestroy(&childMesh);
	meshGLDestroy(&rootMesh);
	sceneDestroyRecursively(&rootNode);
}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
GLchar vertexCode[] = "\
    uniform mat4 viewing;\
    uniform mat4 modeling;\
    attribute vec3 position;\
    attribute vec2 texCoords;\
    attribute vec3 normal;\
    varying vec3 fragPos;\
    varying vec3 normalDir;\
    varying vec2 st;\
    void main() {\
        vec4 worldPos = modeling * vec4(position, 1.0);\
        gl_Position = viewing * worldPos;\
        fragPos = vec3(worldPos);\
        normalDir = vec3(modeling * vec4(normal, 0.0));\
        st = texCoords;\
    }";
GLchar fragmentCode[] = "\
    uniform sampler2D texture0;\
    uniform vec3 specular;\
    uniform vec3 camPos;\
    uniform vec3 lightPos;\
    uniform vec3 lightCol;\
    uniform vec3 lightAtt;\
    uniform float cosAngle;\
    uniform vec3 dir;\
    varying vec3 fragPos;\
    varying vec3 normalDir;\
    varying vec2 st;\
    void main() {\
        vec3 surfCol = vec3(texture2D(texture0, st));\
        vec3 norDir = normalize(normalDir);\
        vec3 litDir = normalize(lightPos - fragPos);\
        vec3 camDir = normalize(camPos - fragPos);\
        vec3 lightNorm = normalize(lightPos);\
        float cos = dot(litDir, lightNorm);\
        vec3 refDir = 2.0 * dot(litDir, norDir) * norDir - litDir;\
        float d = distance(lightPos, fragPos);\
        float a = lightAtt[0] + lightAtt[1] * d + lightAtt[2] * d * d;\
        float diffInt = dot(norDir, litDir) / a;\
        float specInt = dot(refDir, camDir);\
        if (diffInt <= 0.0 || specInt <= 0.0)\
            specInt = 0.0;\
        float ambInt = 0.1;\
        if (diffInt <= ambInt)\
            diffInt = ambInt;\
        vec3 diffLight = diffInt * lightCol * surfCol;\
        float shininess = 64.0;\
        vec3 specLight = pow(specInt / a, shininess) * lightCol * specular;\
        gl_FragColor = vec4(cosAngle,0.0, 0.0,1.0);\
    }";
            // if (cos <= cosAngle)\
            // gl_FragColor = vec4(ambInt, ambInt, ambInt, 1.0);\
	//gl_FragColor = vec4(diffLight + specLight, 1.0);
	program = makeProgram(vertexCode, fragmentCode);
	if (program != 0) {
		glUseProgram(program);
		attrLocs[0] = glGetAttribLocation(program, "position");
		attrLocs[1] = glGetAttribLocation(program, "texCoords");
		attrLocs[2] = glGetAttribLocation(program, "normal");
		lightLocs[0] = glGetAttribLocation(program, "lightPos");
		lightLocs[1] = glGetAttribLocation(program, "lightCol");
		lightLocs[2] = glGetAttribLocation(program, "lightAtt");
		lightLocs[4] = glGetAttribLocation(program, "cosAngle");
		lightLocs[3] = glGetAttribLocation(program, "dir");
		camLoc = glGetAttribLocation(program, "camPos");
		viewingLoc = glGetUniformLocation(program, "viewing");
		modelingLoc = glGetUniformLocation(program, "modeling");
		textureLoc[0] = glGetUniformLocation(program, "texture");
		unifLocs[0] = glGetUniformLocation(program, "specular");
	}
	return (program == 0);
}

void render(void) {
	/* Open program and use camera render*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	camRender(&cam, viewingLoc);
	/* set the animation of rootNode. */
	GLdouble rot[3][3], identity[4][4], axis[3] = {1.0, 1.0, 1.0};
	vecUnit(3, axis, axis);
	alpha += 0.01;
	mat33AngleAxisRotation(alpha, axis, rot);
	sceneSetRotation(&rootNode, rot);
	// sceneSetOneUniform(&rootNode, 0, 0.5 + 0.5 * sin(alpha * 7.0));
	/* This rendering code is different from that in 520mainCamera.c. */
	mat44Identity(identity);
	GLuint unifDims[2] = {1,1};
	GLuint attrDims[3] = {3, 2, 3};
	// set the lighting and call lightrender.
	lightShineFrom(&light, lightTranslation, M_PI/3.0, M_PI/3.0);
	lightSetAttenuation(&light, lightAtten);
	lightSetColor(&light, lightColor);
	lightSetType(&light, lightSPOT);
	lightSetSpotAngle(&light, lightAngle);
	lightRender(&light,lightLocs[0], lightLocs[1], lightLocs[2], lightLocs[3], lightLocs[4]);
	// calculate the cameraPos and pass it to Campos
	GLdouble CameraX = cam.distance * sin(cam.phi) * cos(cam.theta);
    GLdouble CameraY = cam.distance * cos(cam.phi) * sin(cam.theta);
    GLdouble CameraZ = cam.distance * cos(cam.phi);
    GLdouble cameraPos[3] = {CameraX, CameraY, CameraZ};
    GLfloat vec[3];
    vecOpenGL(3, cameraPos, vec);
    //printVector(3, cameraPos);
	glUniform3f(camLoc, vec[0], vec[1], vec[2]);
	sceneRender(&rootNode, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
		attrDims, attrLocs, textureLoc);
}

int main(void) {
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0)
        return 1;
    GLFWwindow *window;
    window = glfwCreateWindow(512, 512, "Scene Graph", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwSetKeyCallback(window, handleKey);
    glfwMakeContextCurrent(window);
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", 
		glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glDepthRange(1.0, 0.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    /* Initialize a whole scene, rather than just one mesh. */
    if (initializeScene() != 0)
    	return 3;
    if (initializeShaderProgram() != 0)
    	return 4;
    GLdouble target[3] = {0.0, 0.0, 0.0};
	camSetControls(&cam, camPERSPECTIVE, M_PI / 6.0, 10.0, 512.0, 512.0, 10.0, 
		M_PI / 4.0, M_PI / 4.0, target);
    while (glfwWindowShouldClose(window) == 0) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    /* Don't forget to destroy the whole scene. */
    destroyScene();
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


