#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform vec2 u_Scale;

out vec2 texCoord;
out vec2 box;

void main()
{
	texCoord = a_TexCoord * u_Scale;

	gl_Position = vec4(a_Position, 1.0f);
}

///////////////////////////////////////////////////////////////////

#type fragment
#version 450 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D u_Texture;
uniform vec2 u_Scale;


// Better, temporally stable box filtering
// [Jimenez14] http://goo.gl/eomGso
// . . . . . . .
// . A . B . C .
// . . D . E . .
// . F . G . H .
// . . I . J . .
// . K . L . M .
// . . . . . . .

vec4 downSample(vec2 uv, vec2 texelSize)
{
    vec4 A = texture(u_Texture, clamp(uv + texelSize * vec2(-2.0,  2.0), vec2(0.0), u_Scale));
    vec4 B = texture(u_Texture, clamp(uv + texelSize * vec2( 0.0,  2.0), vec2(0.0), u_Scale));
    vec4 C = texture(u_Texture, clamp(uv + texelSize * vec2( 2.0,  2.0), vec2(0.0), u_Scale));
    vec4 D = texture(u_Texture, clamp(uv + texelSize * vec2(-1.0,  1.0), vec2(0.0), u_Scale));
    vec4 E = texture(u_Texture, clamp(uv + texelSize * vec2( 1.0,  1.0), vec2(0.0), u_Scale));
    vec4 F = texture(u_Texture, clamp(uv + texelSize * vec2(-2.0,  0.0), vec2(0.0), u_Scale));
    vec4 G = texture(u_Texture, clamp(uv                               , vec2(0.0), u_Scale));
    vec4 H = texture(u_Texture, clamp(uv + texelSize * vec2( 2.0,  0.0), vec2(0.0), u_Scale));
    vec4 I = texture(u_Texture, clamp(uv + texelSize * vec2(-1.0, -1.0), vec2(0.0), u_Scale));
    vec4 J = texture(u_Texture, clamp(uv + texelSize * vec2( 1.0, -1.0), vec2(0.0), u_Scale));
    vec4 K = texture(u_Texture, clamp(uv + texelSize * vec2(-2.0, -2.0), vec2(0.0), u_Scale));
    vec4 L = texture(u_Texture, clamp(uv + texelSize * vec2( 0.0, -2.0), vec2(0.0), u_Scale));
    vec4 M = texture(u_Texture, clamp(uv + texelSize * vec2( 2.0, -2.0), vec2(0.0), u_Scale));

    return (D + E + I + J + G) * 0.125 + (B + F + L + H) * 0.0625 + (A + C + K + M) * 0.03125;
}


void main()
{
	vec2 texel = 1.0 / textureSize(u_Texture, 0);
	fragColor = downSample(texCoord, texel);
}