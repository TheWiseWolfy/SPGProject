#version 400 core
								
layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textCords;

out vec3 outPos;
out vec3 outNormal;
out vec2 outTextCords;

//out vec2 TexCoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 ModelMatrix;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4(positions, 1.0);
	outPos = positions;

	vec4 ajustedNormal = ModelMatrix * vec4( normals, 1.0);
	outNormal =  ajustedNormal.xyz;  

	outTextCords = textCords;
}


