#version 330

// Structs
struct PointLight {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
	float range;
	vec3 att;
};

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular; // w = SpecPower
	float emissive;
};

struct LightOut {
	vec4 ambient;
	vec4 diffuse;
	vec4 spec;
};

struct DirectionalLight {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 direction;
};

// Attributes
in vec2 texCoords;
in vec3 posW;
in vec3 normalW;

uniform sampler2D texture_diffuse;

uniform PointLight pointLight[10];
uniform DirectionalLight dirLight;
uniform Material material;
uniform vec3 eyePosW;
uniform int pointLightCount;

out vec4 out_color;

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
LightOut computeDirectionalLight(Material mat, DirectionalLight L, vec3 normal, vec3 toEye) {
	// Initialize outputs.
	LightOut lightOut;
	lightOut.ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	lightOut.diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	lightOut.spec = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	vec3 lightVec = -L.direction;

	// Add ambient term.
	lightOut.ambient = mat.ambient * L.ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	if (diffuseFactor > 0.0f) {
		vec3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		lightOut.diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		lightOut.spec = specFactor * mat.specular * L.specular;
	}

	return lightOut;
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
LightOut computePointLight(Material mat, PointLight L, vec3 pos, vec3 normal, vec3 toEye)
{
	// Initialize outputs.
	LightOut lightOut;
	lightOut.ambient = vec4(0, 0, 0, 0);
	lightOut.diffuse = vec4(0, 0, 0, 0);
	lightOut.spec 	 = vec4(0, 0, 0, 0);

	// The vector from the surface to the light.
	vec3 lightVec = L.position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > L.range)
		return lightOut;

	// Normalize the light vector.
	lightVec /= d;

	// Ambient term.
	lightOut.ambient = mat.ambient * L.ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	if (diffuseFactor > 0.0f) {
		vec3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		lightOut.diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		lightOut.spec = specFactor * mat.specular * L.specular;
	}

	// Attenuate
	float att = 1.0f / dot(L.att, vec3(1.0f, d, d*d));

	lightOut.diffuse *= att;
	lightOut.spec *= att;

	return lightOut;
}

void main() {
	// Sample texture.
	vec4 texColor = texture(texture_diffuse, texCoords);
	
	if(texColor.a < 0.1)
		discard;
		
	if(material.emissive > 0.1f) {
		out_color = texColor;
		return;
	}
	
	// Interpolating normal can unnormalize it, so normalize it.
	vec3 cNormalW = normalize(normalW);

	vec3 toEyeW = eyePosW - posW;
	
	float distToEye = length(toEyeW);

	toEyeW /= distToEye; // normalize

	// Start with a sum of zero. 
	vec4 ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 spec = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	// Add light contributations
	LightOut dirLightOut = computeDirectionalLight(material, dirLight, cNormalW, toEyeW);
	ambient += dirLightOut.ambient;
	diffuse += dirLightOut.diffuse;
	spec += dirLightOut.spec;
	
	
	for(int i = 0; i < pointLightCount; ++i) {
		LightOut pointLightOut = computePointLight(material, pointLight[i], posW, cNormalW, toEyeW);
		ambient += pointLightOut.ambient;
		diffuse += pointLightOut.diffuse;
		spec += pointLightOut.spec;
	}

	vec4 litColor = texColor*(ambient + diffuse) + spec;

	// Common to take alpha from diffuse material.
	litColor.a = material.diffuse.a * texColor.a;

	out_color = litColor;
}