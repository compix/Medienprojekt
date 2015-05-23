uniform vec4 lightColor;
uniform vec2 lightPos;
uniform float radius;
uniform vec2 screenResolution;
uniform vec3 attenuation;

void main()
{
	vec2 pixelCoord = gl_FragCoord.xy;
	pixelCoord.y = screenResolution.y - pixelCoord.y; // transform to our coordinate system (origin at the top-left corner)
	float dist = length(lightPos-pixelCoord);
	float att = radius/(attenuation.x + attenuation.y*dist + attenuation.z*dist*dist); // dist*dist + radius*0.5f
	
    gl_FragColor = vec4(att, att, att, 1.0) * lightColor;
}