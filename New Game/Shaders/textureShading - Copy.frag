#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;
#define MAX_LIGHTS 128
uniform sampler2D mySampler;
uniform int numLights;
uniform vec2 lightLoc[MAX_LIGHTS];
uniform vec3 lightCol[MAX_LIGHTS];
uniform float lightRad[MAX_LIGHTS];



void main() {
	//cos(x) returns a number between -1 and 1. To convert it into the range 0 to 1
	//you simply do (cos(x) + 1.0) * 0.5
	vec4 lightColor = vec4(1, 1, 1, 1);
	vec4 textureColor = texture(mySampler, fragmentUV);
	int lights = numLights;
	if(numLights > MAX_LIGHTS) lights = MAX_LIGHTS;
	for(int i = 0;i < lights;i++) {
		float distance = length(lightLoc[i] - fragmentPosition.xy)/lightRad[i] + 0.5;
		float attenuation = 1 / pow(distance, 2);
		lightColor = lightColor + vec4(attenuation, attenuation, attenuation, 1) * vec4(lightCol[i], 1);
	}
	//Make crazy colors using time and position!
	color =  fragmentColor * textureColor * lightColor;
}