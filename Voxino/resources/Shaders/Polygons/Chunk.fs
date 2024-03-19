#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
in float TexIndex;
in float v_DirectionalLightning;

uniform sampler2DArray u_TextureArray;

void main()
{
	FragColor = texture(u_TextureArray, vec3(v_TexCoord, TexIndex));
	FragColor.r *= v_DirectionalLightning;
	FragColor.g *= v_DirectionalLightning;
	FragColor.b *= v_DirectionalLightning;
};