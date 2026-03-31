#version 410 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gMaterial;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

struct MaterialData {
    vec3 albedoColor;
    float metallicFactor;
    float roughnessFactor;
    float aoFactor;

    int hasAlbedoMap;
    int hasMetallicMap;
    int hasRoughnessMap;
    int hasAoMap;
    int hasNormalMap;
};

uniform MaterialData u_Material;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_NormalMap;

void main() {
    vec3 albedo = u_Material.albedoColor;
    if (u_Material.hasAlbedoMap == 1) {
        albedo *= texture(u_AlbedoMap, fs_in.TexCoord).rgb;
    }

    float metallic = u_Material.metallicFactor;
    if (u_Material.hasMetallicMap == 1) {
        metallic *= texture(u_MetallicMap, fs_in.TexCoord).r;
    }

    float roughness = u_Material.roughnessFactor;
    if (u_Material.hasRoughnessMap == 1) {
        roughness *= texture(u_RoughnessMap, fs_in.TexCoord).r;
    }

    float ao = u_Material.aoFactor;
    if (u_Material.hasAoMap == 1) {
        ao *= texture(u_AOMap, fs_in.TexCoord).r;
    }

    vec3 N = normalize(fs_in.Normal);

    gPosition = vec4(fs_in.FragPos, 1.0);
    gNormal = vec4(N * 0.5 + 0.5, 1.0);
    gAlbedo = vec4(albedo, 1.0);
    gMaterial = vec4(metallic, roughness, ao, 1.0);
}