#version 330 core

layout(location = 0) in vec3  position;
layout(location = 1) in float textureId;
layout(location = 2) in float face;
layout(location = 3) in float directionalLightning;

out float v_Face;
out float v_TextureId;
out float v_DirectionalLightning;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

void main()
{
    mat4 mvp = projection * view;
    gl_Position = mvp * vec4(position, 1.0);
    v_TextureId = textureId;
    v_Face = face;
    v_DirectionalLightning = directionalLightning;
}