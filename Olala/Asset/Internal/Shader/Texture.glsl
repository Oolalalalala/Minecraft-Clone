#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

out vec4 color;
out vec2 texCoord;
out float texIndex;


void main()
{
	texCoord = a_TexCoord;
	color = a_Color;
	texIndex = a_TexIndex;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}

///////////////////////////////////////////////////////////////////

#type fragment
#version 450 core

out vec4 fragColor;

in vec4 color;
in vec2 texCoord;
in float texIndex;

uniform sampler2D u_Textures[32];

void main()
{
	if (int(texIndex) == -1)
		fragColor = color;
	else
		fragColor = color * texture(u_Textures[int(texIndex)], texCoord);
}