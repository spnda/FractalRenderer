/*
 * spnda, Copyright (c) 2020
 * base shader fragment shader
 */
#version 330 core

layout (location = 0) in vec2 position;

void main() {
	gl_Position = vec4(position, 0.0, 0.0);
}
