// Thanks to http://www.alpha-arts.net/blog/articles/view/30/shader-de-blur

#version 130

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

uniform sampler2D samp;

void main()
{
	vec2 off = vec2(0.008, 0.0);

	color = vec4(fragmentColor * (
		texture(samp, fragmentUV + off * 1.0) * 0.06 +
		texture(samp, fragmentUV + off * 0.75) * 0.09 +
		texture(samp, fragmentUV + off * 0.5) * 0.12 +
		texture(samp, fragmentUV + off * 0.25) * 0.15 +
		texture(samp, fragmentUV) * 0.16 +
		texture(samp, fragmentUV - off * 1.0) * 0.06 +
		texture(samp, fragmentUV - off * 0.75) * 0.09 +
		texture(samp, fragmentUV - off * 0.5) * 0.12 +
		texture(samp, fragmentUV - off * 0.25) * 0.15));
}