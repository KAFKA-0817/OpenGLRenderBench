#version 410 core

out vec4 FragColor;
in vec2 vTexCoord;

struct Light {
    vec3 direction;
    vec3 color;
};

uniform sampler2D u_GPosition;
uniform sampler2D u_GNormal;
uniform sampler2D u_GAlbedo;
uniform sampler2D u_GMaterial;
uniform sampler2D u_GEmissive;

uniform Light u_Light;
uniform vec3 u_ViewPos;

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
    vec3 fragPos = texture(u_GPosition, vTexCoord).rgb;
    vec3 normal = texture(u_GNormal, vTexCoord).rgb * 2.0 - 1.0;
    vec3 albedo = texture(u_GAlbedo, vTexCoord).rgb;
    vec3 material = texture(u_GMaterial, vTexCoord).rgb;
    vec3 emissive = texture(u_GEmissive, vTexCoord).rgb;

    float metallic = material.r;
    float roughness = clamp(material.g, 0.04, 1.0);
    float ao = material.b;

    vec3 N = normalize(normal);
    vec3 V = normalize(u_ViewPos - fragPos);
    vec3 L = normalize(-u_Light.direction);
    vec3 H = normalize(V + L);

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

    float hemi = N.y * 0.5 + 0.5;
    vec3 skyColor = vec3(0.10, 0.12, 0.16);
    vec3 groundColor = vec3(0.13, 0.13, 0.13);
    vec3 hemiLight = mix(groundColor, skyColor, hemi);
    vec3 ambient = hemiLight * albedo * ao;

    vec3 color = ambient + Lo + emissive;
    color = vec3(1.0) - exp(-color * 1.0);
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}