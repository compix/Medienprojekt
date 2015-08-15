#version 330
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec2 in_texCoords; 

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;

void main() {
	gl_Position = proj * view * model * vec4(in_pos, 1.0);
	texCoords = in_texCoords;
}