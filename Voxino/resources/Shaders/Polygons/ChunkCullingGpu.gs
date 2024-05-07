#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 g_TexCoords;
out float g_TextureId;
out float g_DirectionalLightning;

in float v_Face[];
in float v_TextureId[];
in float v_DirectionalLightning[];

float voxSize = 1;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

void faceVertices(int blockFace, out vec3 vertices[4]) {
    switch (blockFace) {
        case 0: // bottom
            vertices[0] = vec3(0, 0, 0); // bottom left close
            vertices[1] = vec3(1, 0, 0); // bottom right close
            vertices[2] = vec3(0, 0, 1); // bottom left far
            vertices[3] = vec3(1, 0, 1); // bottom right far
            break;

        case 1: // top
            vertices[0] = vec3(0, 1, 1); // top far left
            vertices[1] = vec3(1, 1, 1); // top far right
            vertices[2] = vec3(0, 1, 0); // top close left
            vertices[3] = vec3(1, 1, 0); // top close right
            break;

        case 2: // left
            vertices[0] = vec3(0, 0, 0); // left bottom close
            vertices[1] = vec3(0, 0, 1); // left bottom far
            vertices[2] = vec3(0, 1, 0); // left top close
            vertices[3] = vec3(0, 1, 1); // left top far
            break;

        case 3: // right
            vertices[0] = vec3(1, 0, 1); // right bottom far
            vertices[1] = vec3(1, 0, 0); // right bottom close
            vertices[2] = vec3(1, 1, 1); // right top far
            vertices[3] = vec3(1, 1, 0); // right top close
            break;


        case 4: // front
            vertices[0] = vec3(0, 0, 1); // front left bottom
            vertices[1] = vec3(1, 0, 1); // front right bottom
            vertices[2] = vec3(0, 1, 1); // front left top
            vertices[3] = vec3(1, 1, 1); // front right top
            break;

        case 5: // back
            vertices[0] = vec3(1, 0, 0); // back right bottom
            vertices[1] = vec3(0, 0, 0); // back left bottom
            vertices[2] = vec3(1, 1, 0); // back right top
            vertices[3] = vec3(0, 1, 0); // back left top
            break;
    }
}

void AddQuad(vec4 center, mat4 mvp, int blockFace) {
    vec3 faces[4];
    faceVertices(blockFace, faces);

    vec2 texCoords[4] = {
        vec2(0, 1),
        vec2(1, 1),
        vec2(0, 0),
        vec2(1, 0)
    };

    for(int i = 0; i < 4; ++i) {
        gl_Position = center + mvp * vec4(faces[i], 0.0);
        g_TexCoords = texCoords[i];
        g_TextureId = v_TextureId[0];
        g_DirectionalLightning = v_DirectionalLightning[0];
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    vec4 center = gl_in[0].gl_Position;
    mat4 mvp = projection * view;
    AddQuad(center, mvp, int(v_Face[0]));
}