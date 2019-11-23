// Idea from http://www.catalinzima.com/2010/07/my-technique-for-the-shader-based-dynamic-2d-shadows/

#version 130

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

uniform sampler2D samp;
uniform float renderTargetSize;

vec2 Reduce(vec2 TexCoord)
{
	float y = TexCoord.y;
	vec2 max = vec2(1, 1);

	if (TexCoord.x >= 0.5)
	{
		for (float i = renderTargetSize / 2.0; i > 0.0; --i)
		{
			vec2 pos = vec2(i / renderTargetSize, y);
			vec2 color = texture(samp, pos).rg;
			max = min(color, max);

			if (max.x != 1.0 && max.y != 1.0)
				return max;
		}
		return max;
	}
	else
	{
		for (float i = renderTargetSize / 2.0; i < renderTargetSize; ++i)
		{
			vec2 pos = vec2(i / renderTargetSize, y);
			vec2 color = texture(samp, pos).rg;
			max = min(color, max);
			if (max.x != 1.0 && max.y != 1.0)
				return max;
		}
		return max;
	}
}

void main()
{
	vec2 relativeUV = fragmentUV * vec2(renderTargetSize/2.0, 1);
	color = vec4(Reduce(vec2(relativeUV.x, fragmentUV.y)), 0, 1);
}

/*void main()
{
	float y = gl_TexCoord[0].y;
	vec2 max = vec2(1, 1);

	if (gl_FragCoord.x == 0.5)
	{
		for (float i = 0.0; i < renderTargetSize / 2.0; ++i)
		{
			vec2 pos = vec2(i / renderTargetSize, y);
			vec2 color = texture2D(texture, pos).rg;
			max = min(color, max);
		}
	}
	else
	{
		for (float i = renderTargetSize; i > renderTargetSize / 2.0; --i)
		{
			vec2 pos = vec2(i / renderTargetSize, y);
			vec2 color = texture2D(texture, pos).rg;
			max = min(color, max);
		}
	}

	gl_FragColor = vec4(max, 0, 1);
}*/
