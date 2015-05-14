uniform sampler2D texture;
uniform sampler2D lightMap;
uniform vec2 screenResolution;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	vec2 lightCoord = gl_FragCoord.xy;
	lightCoord.x /= screenResolution.x;
	lightCoord.y /= screenResolution.y;
	vec4 light = texture2D(lightMap, lightCoord);
	
    gl_FragColor = gl_Color * pixel * light + pixel*0.8f; //vec4(result, pixel.a);
}