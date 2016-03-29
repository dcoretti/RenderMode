#version 430 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 mvp = mat4(1.0);


void main () {
	gl_Position = mvp * pos;
}