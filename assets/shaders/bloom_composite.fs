#version 410 core

in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D u_SceneTexture;
uniform sampler2D u_BloomTexture;
uniform float u_BloomStrength;

void main() {
    vec3 scene = texture(u_SceneTexture, vTexCoords).rgb;
    vec3 bloom = texture(u_BloomTexture, vTexCoords).rgb;
    FragColor = vec4(scene + bloom * u_BloomStrength, 1.0);
}
