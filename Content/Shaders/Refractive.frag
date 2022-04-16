#version 330 core

in VS_OUT 
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

uniform vec2 screenSize;
uniform vec2 refractionScale;

uniform sampler2D normalMap;
uniform sampler2D screenColors;

void main()
{   
	// The actual screen UV of this pixel
	// “screenSize” is holding (windowWidth, windowHeight)
	vec2 screenUV = gl_FragCoord.xy / screenSize;

	// Use object’s normal map as an offset
	vec2 offsetUV = texture(normalMap, fs_in.texCoords).xy * 2 - 1;
	offsetUV.y *= -1;  
	
	// UV's are upside down compared to world space
	// Distort the screen UV by the offset, scaling as necessary
	vec2 refractedUV = screenUV + offsetUV * refractionScale;

	FragColor = texture(screenColors, refractedUV);
	//FragColor = texture(screenColors, screenUV);
}