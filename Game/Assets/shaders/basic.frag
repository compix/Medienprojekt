#version 330

// Attributes
in vec2 texCoords;

uniform sampler2D texture_diffuse;

layout(location = 0) out vec4 out_color;

void main() {
	out_color = texture(texture_diffuse, texCoords);
	if(out_color.a <= 0.05)
		discard;
}