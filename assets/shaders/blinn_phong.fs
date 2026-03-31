#version 410 core

out vec4 FragColor;

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

struct Light {
    vec3 direction;
    vec3 color;
};

struct MaterialData {
    vec3 albedo;
    vec3 specular;
    float shininess;
    int hasDiffuseMap;
    int hasSpecularMap;
};

uniform Light u_Light;
uniform MaterialData u_Material;
uniform vec3 u_ViewPos;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-u_Light.direction);
    vec3 V = normalize(u_ViewPos - vFragPos);
    vec3 H = normalize(L + V);

    vec3 albedo = u_Material.albedo;
    if (u_Material.hasDiffuseMap == 1) {
        albedo *= texture(u_DiffuseMap, vTexCoord).rgb;
    }

    vec3 specularColor = u_Material.specular;
    if (u_Material.hasSpecularMap == 1) {
        specularColor *= texture(u_SpecularMap, vTexCoord).rgb;
    }

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), u_Material.shininess);

    vec3 ambient = 0.05 * albedo;
    vec3 diffuse = diff * albedo * u_Light.color;
    vec3 specular = spec * specularColor * u_Light.color;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}