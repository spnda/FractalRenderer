#version 460 core

layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 outColour;

layout(binding = 0) uniform sampler2D colorTex;

void main() {
    vec4 color = texture(colorTex, uv);
    outColour = color;
}
