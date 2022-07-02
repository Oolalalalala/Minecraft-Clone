#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform vec2 u_Offset = vec2(0.0, 0.0);
uniform vec2 u_Scale= vec2(1.0, 1.0);

out vec2 texCoord;

void main()
{
	texCoord = u_Offset + a_TexCoord * u_Scale;

	gl_Position = vec4(a_Position, 1.0f);
}

///////////////////////////////////////////////////////////////////

#type fragment
#version 450 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D u_Texture;

void main()
{
	fragColor = texture(u_Texture, texCoord);
}