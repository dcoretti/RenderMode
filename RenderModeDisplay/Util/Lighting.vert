#version 430 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out vec2 uvCoord;
out vec4 normalView;
out vec4 posView;

out vec3 posWorld;
out vec3 lightSourceDirView;
out vec3 eyeDirView;
out vec4 posProj;
out float lightVertexDist;

uniform mat4 mvp = mat4(1.0);
uniform mat4 mv = mat4(1.0);
uniform mat4 m = mat4(1.0);
uniform mat4 v;


uniform LightSource {
    vec3 diffuse;
    vec3 specular;
    vec3 location;
} lightSource;

void main() {
	
	posWorld = (m * pos).xyz;
	vec3 posCamera = (mv * pos).xyz;
	eyeDirView = vec3(0,0,0) - posCamera;	// camera is at origin in its own space
	vec3 lightSourcePosView = (v * vec4(lightSource.location, 1)).xyz;

	lightSourceDirView = lightSourcePosView + eyeDirView;

    uvCoord = uv;
	//normalView = vec4(normal,1);
    normalView = normalize(v * vec4(normal,1));
    posView = mv * pos;
	lightVertexDist = distance(lightSourcePosView, vec3(0,0,0));
    gl_Position = mvp * pos;
}