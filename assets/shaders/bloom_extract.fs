#version 410 core

in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform float u_Threshold;

void main() {
    vec3 color = texture(u_SceneTexture, vTexCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    vec3 bloom = brightness > u_Threshold ? color : vec3(0.0);
    FragColor = vec4(bloom, 1.0);
}
