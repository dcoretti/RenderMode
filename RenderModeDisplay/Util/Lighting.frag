#version 430 core

in vec2 uv;
in vec4 normal;	// camera space (normalized)
in vec4 posView;
uniform mat4 v;
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
	vec3 lightSourceView = (v * vec4(lightSource.location, 1)).xyz;
//texture(texSampler, uv).rgb 
    fragColor = vec3(1.0,1.0,1.0) * clamp(dot(normal.xyz, lightSourceView),0,1);
};
