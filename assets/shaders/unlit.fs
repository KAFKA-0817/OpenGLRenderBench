#version 410 core

out vec4 FragColor;

in vec2 vTexCoord;

struct MaterialData {
    vec3 baseColor;
    int hasAlbedoMap;
};

uniform MaterialData u_Material;
uniform sampler2D u_AlbedoMap;

void main() {
    vec3 color = u_Material.baseColor;
    if (u_Material.hasAlbedoMap == 1) {
        color *= texture(u_AlbedoMap, vTexCoord).rgb;
    }
    FragColor = vec4(color, 1.0);
}