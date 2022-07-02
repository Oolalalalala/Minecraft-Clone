#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;


out vec2 texCoord;

void main()
{
	texCoord = a_TexCoord;

	gl_Position = vec4(a_Position, 1.0f);
}

///////////////////////////////////////////////////////////////////

#type fragment
#version 450 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D u_Texture;


float w(float x, float a, float b)
{
	float v = (x - a) / (b - a);
	return clamp(v * v * (3 - 2 * v), 0, 1);
}

float h(float x, float a, float b)
{
    return 
}

f(x) = 

void main()
{
	vec4 color = texture(u_Texture, texCoord);
	fragColor = vec4(aces(color.rgb), 1.0);
	//fragColor = vec4(1.0);
}