// Idea from http://www.catalinzima.com/2010/07/my-technique-for-the-shader-based-dynamic-2d-shadows/

#version 130

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

uniform sampler2D reduce;
uniform sampler2D samp;
uniform float renderTargetSize;
uniform vec4 lightColor;


float GetShadowDistanceH(vec2 TexCoord)
{
	float u = TexCoord.x;
	float v = TexCoord.y;

	u = abs(u - 0.5) * 2.0;
	v = v * 2.0 - 1.0;
	float v0 = v / u;
	v0 = (v0 + 1.0) / 2.0;
	if (u == v) {
		TexCoord.x = 0;
	}

	// Horizontal info was stored in the Red component
	return texture(reduce, vec2((TexCoord.x < 0.5) ? 0.5 : 1.0, v0)).r;
}

float GetShadowDistanceV(vec2 TexCoord)
{
	float u = TexCoord.y;
	float v = TexCoord.x;

	u = abs(u - 0.5) * 2.0;
	v = v * 2.0 - 1.0;
	float v0 = v / u;
	v0 = (v0 + 1.0) / 2.0;
	
	// Vertical info was stored in the Green component
	return texture(reduce, vec2((TexCoord.y < 0.5) ? 0.5 : 1.0, v0)).g;
}

void main()
{
	// Distance of this pixel from the center
	float dist = distance(fragmentUV, vec2(0.5, 0.5));
	//dist -= 2.0 / renderTargetSize;

	// Distance stored in the shadow map
	float shadowMapDistance;
	// Coords in [-1,1]
	float nY = 2.0 * (fragmentUV.y - 0.5);
	float nX = 2.0 * (fragmentUV.x - 0.5);

	// We use these to determine which quadrant we are in
	if (abs(nY) < abs(nX))
	{
		shadowMapDistance = GetShadowDistanceH(fragmentUV);
	}
	else
	{
		shadowMapDistance = GetShadowDistanceV(fragmentUV);
	}
	if (nX == nY && nX >= 0) {
		shadowMapDistance = GetShadowDistanceH(vec2(fragmentUV.x, fragmentUV.y - 0.00001));
	}
	if (nX == -nY && nX >= 0) {
		shadowMapDistance = GetShadowDistanceH(vec2(fragmentUV.x, fragmentUV.y + 0.00001));
	}

	//float light = 1;
	//vec4 texCol = texture(samp, fragmentUV);
	//if (dist > shadowMapDistance) {
	//	float test = abs(dist - shadowMapDistance);
	//	if (test < 0.025) {
	//		texCol.a = 1;
	//	} else {
	//		texCol.rgb = vec3(0, 0, 0);
	//	}
	//	light = 0;
	//}
	//vec4 result = vec4(light + texCol.r, light + texCol.g, light + texCol.b, 1);
	// If distance to this pixel is lower than distance from shadowMap,
	// then we are not in shadow
	float light = dist <= shadowMapDistance ? 1.0 : 0.0;
	vec4 result = vec4(light, light, light, 1);

	float x = 2*dist;
	//result.a = 0.5 - dist;
	result.a = 1.0 / (2*(x + 0.37)) - 0.37;
	//result.a = 1.0 / ((10 * x + 0.7)) - 0.225 + x * 0.1;
	//result.a = 1.0 / ((2*x + 0.64)) - 0.64;
	//result.a = 1 - 4*pow(x, 2);
	//result.a = 1 - 25*pow(x - 0.3, 3) - 0.6 - x;
	result *= lightColor;
	color = result;
}
