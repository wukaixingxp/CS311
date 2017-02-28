			vec3 diffuse = vec3(texture(texture0, st));\
			vec3 litDir = normalize(lightPos - fragPos);\
			vec3 pinkDir = normalize(pinkPos - fragPos);\
			float diffInt, pinkInt,specInt = 0.0;\
			if (dot(lightAim, -litDir) < lightCos)\
				diffInt = 0.0;\
			else\
				diffInt = 1.0;\
			if (dot(pinkAim, -pinkDir) < pinkCos)\
				pinkInt = 0.0;\
			else\
				pinkInt = 1.0;\
			float sdw = textureProj(textureSdw, fragSdw);\
			float pink = textureProj(texturePink, fragPink);\
			diffInt *= sdw;\
			specInt *= sdw;\
			pinkInt *= pink;\
			pinkInt *= pink;\
			vec3 diffRefl = max(0.2, diffInt) * lightCol * diffuse;\
			vec3 pinkRefl = max(0.2, pinkInt) * pinkCol * diffuse;\
			vec3 specRefl = specInt * lightCol * specular;\
			fragColor = vec4(diffRefl + specRefl, 1.0);\

				GLchar vertexCode[] = "\
		#version 140\n\
		uniform mat4 viewing;\
		uniform mat4 modeling;\
		uniform mat4 viewingSdw;\
		uniform mat4 viewingPink;\
		in vec3 position;\
		in vec2 texCoords;\
		in vec3 normal;\
		out vec3 fragPos;\
		out vec3 normalDir;\
		out vec2 st;\
		out vec4 fragSdw;\
		out vec4 fragPink;\
		void main(void) {\
			mat4 scaleBias = mat4(\
				0.5, 0.0, 0.0, 0.0, \
				0.0, 0.5, 0.0, 0.0, \
				0.0, 0.0, 0.5, 0.0, \
				0.5, 0.5, 0.5, 1.0);\
			vec4 worldPos = modeling * vec4(position, 1.0);\
			gl_Position = viewing * worldPos;\
			fragSdw = scaleBias * viewingSdw * worldPos;\
			fragPink = scaleBias * viewingPink * worldPos;\
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
		uniform vec3 lightAim;\
		uniform float lightCos;\
		uniform vec3 pinkPos;\
		uniform vec3 pinkCol;\
		uniform vec3 pinkAtt;\
		uniform vec3 pinkAim;\
		uniform float pinkCos;\
		uniform sampler2DShadow textureSdw;\
		uniform sampler2DShadow texturePink;\
		in vec3 fragPos;\
		in vec3 normalDir;\
		in vec2 st;\
		in vec4 fragSdw;\
		in vec4 fragPink;\
		out vec4 fragColor;\



			program = makeProgram(vertexCode, fragmentCode);
	if (program != 0) {
		glUseProgram(program);
		attrLocs[0] = glGetAttribLocation(program, "position");
		attrLocs[1] = glGetAttribLocation(program, "texCoords");
		attrLocs[2] = glGetAttribLocation(program, "normal");
		viewingLoc = glGetUniformLocation(program, "viewing");
		modelingLoc = glGetUniformLocation(program, "modeling");
		unifLocs[0] = glGetUniformLocation(program, "specular");
		textureLocs[0] = glGetUniformLocation(program, "texture0");
		camPosLoc = glGetUniformLocation(program, "camPos");
		lightPosLoc = glGetUniformLocation(program, "lightPos");
		lightColLoc = glGetUniformLocation(program, "lightCol");
		lightAttLoc = glGetUniformLocation(program, "lightAtt");
		lightDirLoc = glGetUniformLocation(program, "lightAim");
		lightCosLoc = glGetUniformLocation(program, "lightCos");
		viewingSdwLoc = glGetUniformLocation(program, "viewingSdw");
		textureSdwLoc = glGetUniformLocation(program, "textureSdw");
		texturePinkLoc = glGetUniformLocation(program, "texturePink");
		viewingPinkLoc = glGetUniformLocation(program, "viewingPink");
		pinkPosLoc = glGetUniformLocation(program, "pinkPos");
		pinkColLoc = glGetUniformLocation(program, "pinkCol");
		pinkAttLoc = glGetUniformLocation(program, "pinkAtt");
		pinkDirLoc = glGetUniformLocation(program, "pinkAim");
		pinkCosLoc = glGetUniformLocation(program, "pinkCos");
	}