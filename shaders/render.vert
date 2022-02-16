#version 460 core

layout(location = 0) out gl_PerVertex {
    vec4 gl_Position;
};
layout(location = 1) out vec2 fs_uv;

void main() {
    float x = -1.0f + float((gl_VertexID & 1) << 2);
    float y = -1.0f + float((gl_VertexID & 2) << 1);

    fs_uv = vec2((x + 1.0f) * 0.5f, (y + 1.0f) * 0.5f);
    gl_Position = vec4(x, y, 0.0f, 1.0f);
}
