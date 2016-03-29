#version 430 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

void main() {
	gl_Position = pos;
}