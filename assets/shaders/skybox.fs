#version 450 core

in vec3 vTexCoord;
layout(location = 0) out vec4 FragColor;

uniform samplerCube u_EnvironmentMap;
uniform float u_Lod;

void main() {
    vec3 color = textureLod(u_EnvironmentMap, normalize(vTexCoord), u_Lod).rgb;
    FragColor = vec4(color, 1.0);
}
