#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection; // Without translation of [View]

out vec3 texCoord;

void main()
{
	texCoord = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}



#type fragment
#version 450 core

out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube u_SkyBox;
uniform vec4 u_Color;

void main()
{
	fragColor = texture(u_SkyBox, texCoord) * u_Color;
}