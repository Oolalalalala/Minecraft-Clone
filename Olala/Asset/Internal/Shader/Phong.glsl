#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};


out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
out vec3 cameraPos;

uniform mat4 u_Model;


void main()
{
	normal = normalize(a_Normal);
	fragPos = vec3(u_Model * vec4(a_Position, 1.0f));
	texCoord = a_TexCoord;
	cameraPos = u_CameraPosition.xyz;

	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0f);
}

///////////////////////////////////////////////////////////////////

#type fragment
#version 460 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in vec3 cameraPos;


uniform vec4 u_Color;
uniform float u_Metallic;
uniform float u_Roughness;
uniform sampler2D u_Texture;
layout(std140, binding = 1) uniform lighting
{
	vec4 u_DirLightDir; // Has a [castShader] flags store in w component
	vec4 u_DirLightColor;
};

void main()
{
	// Ambient
	float ambient = 0.1f;

	// Diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(vec3(u_DirLightDir));
	float diffuse = max(dot(norm, -lightDir), 0.f);

	// Specular 
	float specularStrength = 1.f;
	vec3 viewDir = normalize(fragPos - cameraPos);
	vec3 reflectDir = reflect(lightDir, norm);
	float specular = pow(max(dot(-viewDir, reflectDir), 0.f), 32) * specularStrength;

	vec3 lighting = (ambient + diffuse + specular) * u_DirLightColor.rgb;
	fragColor = vec4(lighting, 1.0) * texture(u_Texture, texCoord) * u_Color;
}