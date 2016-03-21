#version 430 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out vec2 outUv;
out vec4 outNormal;
out vec4 outPosView;

uniform mat4 mvp = mat4(1.0);
uniform mat4 mv = mat4(1.0);
uniform mat4 m = mat4(1.0);

void main() {
    gl_Position = mvp * pos;
    outUv = uv;
    outNormal = normalize(mv * vec4(normal,1));
    outPosView = mv * pos;
};