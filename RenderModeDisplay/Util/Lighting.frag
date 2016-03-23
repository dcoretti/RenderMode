#version 430 core

//in vec2 uvCoord;
//in vec4 normalView;	// camera space (normalized)
//in vec4 posView;
//in vec3 posWorld;
//in vec3 lightSourceDirView;
//in vec3 eyeDirView;

out vec3 fragColor;

uniform sampler2D texSampler;


uniform LightSource {
    vec3 diffuse;
    vec3 specular;
    vec3 location;
} lightSource;


uniform Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} material;

void main() {
//texture(texSampler, uvCoord).rgb 
//material.diffuse * lightSource.diffuse *
    fragColor =  lightSource.diffuse;// normalView.xyz; //vec3(1.0,0.0,0.0) * clamp(dot(normal.xyz, lightSourceDirView),0,1);
}
