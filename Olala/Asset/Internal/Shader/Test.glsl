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
uniform float u_Lod;

void main()
{
	fragColor = textureLod(u_Texture, texCoord, u_Lod);
}