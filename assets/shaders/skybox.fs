#version 450 core

in vec3 vTexCoord;
layout(location = 0) out vec4 FragColor;

uniform samplerCube u_EnvironmentMap;

void main() {
    vec3 color = texture(u_EnvironmentMap, normalize(vTexCoord)).rgb;
    FragColor = vec4(color, 1.0);
}
