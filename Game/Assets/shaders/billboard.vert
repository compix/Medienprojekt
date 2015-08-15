#version 330
layout(location = 1) in vec3 in_pos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;

void main() {
	vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);
	
	vec3 posW = (model * vec4(cameraRight * in_pos.x + cameraUp * in_pos.y, 1.0)).xyz;
	gl_Position = proj * view * vec4(posW, 1.0);
	
	texCoords = vec2(in_pos.x, in_pos.y) + vec2(0.5, 0.5);
}