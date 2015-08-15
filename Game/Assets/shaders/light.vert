#version 330
in vec4 in_pos;
in vec3 in_normal; 
in vec2 in_texCoords; 

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 modelInvTranspose;

out vec2 texCoords;
out vec3 posW;
out vec3 normalW;
						
void main() {
	gl_Position = proj * view * model * in_pos;
	
	posW = (model * in_pos).xyz;
	normalW = mat3(modelInvTranspose) * in_normal;
	
	texCoords = in_texCoords;
}