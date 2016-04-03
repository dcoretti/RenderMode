#version 430 core

in vec2 uvCoord;
in vec4 normalView;	// camera space (normalized)
in vec4 posView;
in vec3 posWorld;
in vec3 lightSourceDirView;
in vec3 eyeDirView;
in float lightVertexDist;

out vec3 fragColor;

uniform sampler2D diffuseTexture;
//uniform sampler2D normalMap;


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
	fragColor = texture(diffuseTexture, uvCoord).rgb;
//material.diffuse * lightSource.diffuse *
    //fragColor = material.diffuse * 
				//lightSource.diffuse * 
				//clamp(dot(normalView.xyz, lightSourceDirView),0,1) / (lightVertexDist * lightVertexDist);
}
