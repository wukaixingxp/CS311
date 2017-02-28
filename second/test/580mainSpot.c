


/* On macOS, compile with...
	clang 580mainSpot.c /usr/local/gl3w/src/gl3w.o -lglfw -framework OpenGL -framework CoreFoundation
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "500shader.c"
#include "530vector.c"
#include "580mesh.c"
#include "520matrix.c"
#include "520camera.c"
#include "540texture.c"
#include "580scene.c"
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

/* create all the global attributes. */
GLuint attrDims[3] = {3, 2, 3};
GLdouble alpha = 0.0;
GLdouble lightTranslation[3] = {-0.5, -0.5, 0};
GLdouble lightColor[3] = {1.0, 1.0, 1.0};
GLdouble lightAtten[3] = {1.0, 0.0, 0.0};
GLdouble lightAngle = M_PI/4.0;

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

    if (texInitializeFile(&texChild, "testChild.jpg", GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT) != 0) {
        glfwTerminate();
        return 3;
       }
    if (texInitializeFile(&texSibling, "testSibling.jpg", GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT) != 0) {
        glfwTerminate();
        return 3;
    }
    if (texInitializeFile(&texRoot, "testRoot.jpg", GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT) != 0) {
        glfwTerminate();
        return 3;
    }
	/* Initialize meshes. */
	meshMesh mesh;
	if (meshInitializeCapsule(&mesh, 0.5, 2.0, 16, 32) != 0)
		return 1;
	meshGLInitialize(&rootMesh, &mesh,3, attrDims, 1);
	meshGLVAOInitialize(&rootMesh, 0, attrLocs);
	meshDestroy(&mesh);
	if (meshInitializeBox(&mesh, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != 0)
		return 2;
	meshGLInitialize(&childMesh, &mesh, 3, attrDims,1);
	meshGLVAOInitialize(&childMesh, 0, attrLocs);
	meshDestroy(&mesh);
	if (meshInitializeSphere(&mesh, 0.5, 16, 32) != 0)
		return 3;
	meshGLInitialize(&siblingMesh, &mesh,3, attrDims, 1);
	meshGLVAOInitialize(&siblingMesh, 0, attrLocs);
	meshDestroy(&mesh);
	/* Initialize scene graph nodes. */
	if (sceneInitialize(&siblingNode, 3, 1, &siblingMesh, NULL, NULL) != 0)
		return 4;
	if (sceneInitialize(&childNode, 3, 1,&childMesh, NULL, NULL) != 0)
		return 5;
	if (sceneInitialize(&rootNode, 3, 1,&rootMesh, &childNode, &siblingNode) != 0)
		return 6;
	/* Customize the uniforms. */
	GLdouble trans[3] = {1.0, 0.0, 0.0};
	sceneSetTranslation(&childNode, trans);
	vecSet(3, trans, 0.0, 1.0, 0.0);
	sceneSetTranslation(&siblingNode, trans);
	GLdouble unif[3] = {1.0,1.0,1.0};
	sceneSetUniform(&siblingNode, unif);
	sceneSetUniform(&childNode, unif);
	sceneSetUniform(&rootNode, unif);
    texTexture *texChildList[1] = {&texChild};
    texTexture *texRootList[1] = {&texRoot};
    texTexture *texSiblingList[1] = {&texSibling};
	sceneSetTexture(&rootNode, texRootList);	
	sceneSetTexture(&childNode, texChildList);
	sceneSetTexture(&siblingNode, texSiblingList);
	return 0;
}

void destroyScene(void) {
	meshGLDestroy(&siblingMesh);
	meshGLDestroy(&childMesh);
	meshGLDestroy(&rootMesh);
    texDestroy(&texChild);
    texDestroy(&texSibling);
    texDestroy(&texRoot);
	sceneDestroyRecursively(&rootNode);
}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
GLchar vertexCode[] = "\
	#version 140\n\
	uniform mat4 viewing;\
    uniform mat4 modeling;\
    in vec3 position;\
    in vec2 texCoords;\
    in vec3 normal;\
    out vec3 fragPos;\
    out vec3 normalDir;\
    out vec2 st;\
    void main() {\
        vec4 worldPos = modeling * vec4(position, 1.0);\
        gl_Position = viewing * worldPos;\
        fragPos = vec3(worldPos);\
        normalDir = vec3(modeling * vec4(normal, 0.0));\
        st = texCoords;\
    }";
GLchar fragmentCode[] = "\
	#version 140\n\
	uniform sampler2D texture0;\
    uniform vec3 specular;\
    uniform vec3 camPos;\
    uniform vec3 lightPos;\
    uniform vec3 lightCol;\
    uniform vec3 lightAtt;\
    uniform vec3 dir;\
    uniform float cosAngle;\
    in vec3 fragPos;\
    in vec3 normalDir;\
    in vec2 st;\
    out vec4 fragColor;\
    void main() {\
        vec3 surfCol = vec3(texture(texture0, st));\
        vec3 lightNorm = normalize(lightPos);\
        vec3 spotDir = normalize(lightPos - fragPos);\
        vec3 norDir = normalize(normalDir);\
        float cos = dot(spotDir,-dir);\
        vec3 litDir = normalize(lightPos - fragPos);\
        vec3 camDir = normalize(camPos - fragPos);\
        vec3 refDir = 2.0 * dot(litDir, norDir) * norDir - litDir;\
        float d = distance(lightPos, fragPos);\
        float a = lightAtt[0] + lightAtt[1] * d + lightAtt[2] * d * d;\
        float diffInt = dot(norDir, litDir)/a;\
        float specInt = dot(refDir, camDir);\
        if (diffInt <= 0.0 || specInt <= 0.0)\
            specInt = 0.0;\
        float ambInt = 0.1;\
        if (diffInt <= ambInt)\
            diffInt = ambInt;\
        vec3 diffLight = diffInt * lightCol * surfCol;\
        float shininess = 64.0;\
        vec3 specLight = pow(specInt / a, shininess) * lightCol * specular;\
        if (cos >= cosAngle){\
        	fragColor = vec4(diffLight+specLight, 1.0);\
        }else{\
            fragColor = vec4(diffLight+specLight,1.0)*0.2;\
        }\
    }";
	program = makeProgram(vertexCode, fragmentCode);
	if (program != 0) {
		glUseProgram(program);
		attrLocs[0] = glGetAttribLocation(program, "position");
		attrLocs[1] = glGetAttribLocation(program, "texCoords");
		attrLocs[2] = glGetAttribLocation(program, "normal");
		lightLocs[0] = glGetUniformLocation(program, "lightPos");
		lightLocs[1] = glGetUniformLocation(program, "lightCol");
		lightLocs[2] = glGetUniformLocation(program, "lightAtt");
		lightLocs[4] = glGetUniformLocation(program, "cosAngle");
		lightLocs[3] = glGetUniformLocation(program, "dir");
		camLoc = glGetUniformLocation(program, "camPos");
		viewingLoc = glGetUniformLocation(program, "viewing");
		modelingLoc = glGetUniformLocation(program, "modeling");
		textureLoc[0] = glGetUniformLocation(program, "texture0");
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
	GLuint unifDims[1] = {3};
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
	sceneRender(&rootNode, identity, modelingLoc, 1, unifDims, unifLocs, 0,textureLoc);
}


int main(void) {
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0) {
    	fprintf(stderr, "main: glfwInit failed.\n");
        return 1;
    }
    /* Ask GLFW to supply an OpenGL 3.2 context. */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window;
    window = glfwCreateWindow(768, 512, "Learning OpenGL 3.2", NULL, NULL);
    if (window == NULL) {
    	fprintf(stderr, "main: glfwCreateWindow failed.\n");
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwMakeContextCurrent(window);
    /* You might think that getting an OpenGL 3.2 context would make OpenGL 3.2 
    available to us. But you'd be wrong. The following call 'loads' a bunch of 
    OpenGL 3.2 functions, so that we can use them. This is why we use GL3W. */
    if (gl3wInit() != 0) {
    	fprintf(stderr, "main: gl3wInit failed.\n");
    	glfwDestroyWindow(window);
    	glfwTerminate();
    	return 3;
    }
    /* We rarely invoke any GL3W functions other than gl3wInit. But just for an 
    educational example, let's ask GL3W about OpenGL support. */
	if (gl3wIsSupported(3, 2) == 0)
		fprintf(stderr, "main: OpenGL 3.2 is not supported.\n");
	else
		fprintf(stderr, "main: OpenGL 3.2 is supported.\n");
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", 
		glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glDepthRange(1.0, 0.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    /* Initialize the shader program before the mesh, so that the shader 
    locations are already set up by the time the vertex array object is 
    initialized. */

    if (initializeShaderProgram() != 0)
    	return 3;
    if (initializeScene() != 0)
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
    /* Delete not just the buffers, but also the vertex array object. */
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
