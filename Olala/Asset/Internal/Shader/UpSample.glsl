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
uniform float u_Radius;


vec4 upSample(vec2 uv, vec2 texelSize)
{
    vec4 d = texelSize.xyxy * vec4(1.0, 1.0, -1.0, 0.0);

    vec4 s;
    s =  texture(u_Texture, uv - d.xy);
    s += texture(u_Texture, uv - d.wy) * 2.0;
    s += texture(u_Texture, uv - d.zy);

    s += texture(u_Texture, uv + d.zw) * 2.0;
    s += texture(u_Texture, uv       ) * 4.0;
    s += texture(u_Texture, uv + d.xw) * 2.0;

    s += texture(u_Texture, uv + d.zy);
    s += texture(u_Texture, uv + d.wy) * 2.0;
    s += texture(u_Texture, uv + d.xy);

    return s * 0.0625;
}


void main()
{
    vec2 texel = 1.0 / textureSize(u_Texture, 0);
	fragColor = upSample(texCoord, texel * u_Radius);
}