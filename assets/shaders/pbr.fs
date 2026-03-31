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

uniform Light u_Light;
uniform MaterialData u_Material;
uniform vec3 u_ViewPos;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_NormalMap;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / max(denom, 0.000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 0.000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(u_ViewPos - vFragPos);
    vec3 L = normalize(-u_Light.direction);
    vec3 H = normalize(V + L);

    vec3 albedo = u_Material.albedoColor;
    if (u_Material.hasAlbedoMap == 1) {
        albedo *= texture(u_AlbedoMap, vTexCoord).rgb;
    }

    float metallic = u_Material.metallicFactor;
    if (u_Material.hasMetallicMap == 1) {
        metallic *= texture(u_MetallicMap, vTexCoord).r;
    }

    float roughness = u_Material.roughnessFactor;
    if (u_Material.hasRoughnessMap == 1) {
        roughness *= texture(u_RoughnessMap, vTexCoord).r;
    }

    float ao = u_Material.aoFactor;
    if (u_Material.hasAoMap == 1) {
        ao *= texture(u_AOMap, vTexCoord).r;
    }

    metallic = clamp(metallic, 0.0, 1.0);
    roughness = clamp(roughness, 0.04, 1.0);
    ao = clamp(ao, 0.0, 1.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    vec3 radiance = u_Light.color;

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
    vec3 ambient = 0.03 * albedo * ao;

    vec3 color = ambient + Lo;
    FragColor = vec4(color, 1.0);
}