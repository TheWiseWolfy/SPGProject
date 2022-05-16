#version 400 core
								
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 outPoz;
out vec3 outColor;
out vec3 outNormal;

//out vec2 TexCoord;

uniform mat4 modelViewProjectionMatrix;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4(aPos, 1.0);
	outPoz = aPos;
    outColor = aColor;
	outNormal = aNormal;

   // TexCoord = aTexCoord;
}


