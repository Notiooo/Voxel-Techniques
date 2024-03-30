#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 g_TexCoords;
in float g_TextureId;
in float g_DirectionalLightning;

uniform sampler2DArray u_TextureArray;

void main()
{
	FragColor = texture(u_TextureArray, vec3(g_TexCoords, g_TextureId));
	FragColor.r *= g_DirectionalLightning;
	FragColor.g *= g_DirectionalLightning;
	FragColor.b *= g_DirectionalLightning;
};