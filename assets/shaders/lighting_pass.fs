#version 410 core

out vec4 FragColor;
in vec2 vTexCoord;

const float PI = 3.14159265359;
const int MAX_POINT_LIGHTS = 64;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    float range;
    vec3 color;
    float intensity;
};

uniform sampler2D u_GPosition;
uniform sampler2D u_GNormal;
uniform sampler2D u_GAlbedo;
uniform sampler2D u_GMaterial;
uniform sampler2D u_GEmissive;
uniform sampler2D u_SSAOMap;
uniform sampler2D u_ShadowMap;

uniform int u_HasDirectionalLight;
uniform DirectionalLight u_DirectionalLight;
uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform vec3 u_ViewPos;
uniform mat4 u_lightSpaceMatrix;

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

vec3 EvaluatePbrDirect(vec3 N,
                       vec3 V,
                       vec3 L,
                       vec3 radiance,
                       vec3 albedo,
                       float metallic,
                       float roughness) {
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
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float ComputePointAttenuation(PointLight light, float distanceToLight) {
    if (light.range <= 0.0001 || distanceToLight >= light.range) {
        return 0.0;
    }

    float normalizedDistance = clamp(distanceToLight / light.range, 0.0, 1.0);
    float falloff = 1.0 - normalizedDistance;
    return falloff * falloff;
}

float ComputeDirectionLightShadowFactor(vec3 fragPos){
    vec4 lightSpacePos = u_lightSpaceMatrix * vec4(fragPos,1.0);
    lightSpacePos /= lightSpacePos.w;
    lightSpacePos = lightSpacePos * 0.5 + 0.5;
    if (lightSpacePos.x < 0.0 || lightSpacePos.x > 1.0 ||
        lightSpacePos.y < 0.0 || lightSpacePos.y > 1.0 ||
        lightSpacePos.z < 0.0 || lightSpacePos.z > 1.0) {
        return 1.0;
    }

    float curDepth = lightSpacePos.z;
    float sampleDepth = texture(u_ShadowMap,vec2(lightSpacePos.x,lightSpacePos.y)).r;
    if(curDepth <= sampleDepth+0.005) return 1.0;
    return 0.0;
}

void main() {
    vec3 fragPos = texture(u_GPosition, vTexCoord).rgb;
    vec3 normal = texture(u_GNormal, vTexCoord).rgb * 2.0 - 1.0;
    vec3 albedo = texture(u_GAlbedo, vTexCoord).rgb;
    vec3 material = texture(u_GMaterial, vTexCoord).rgb;
    vec3 emissive = texture(u_GEmissive, vTexCoord).rgb;
    float ssao = texture(u_SSAOMap, vTexCoord).r;

    float metallic = material.r;
    float roughness = clamp(material.g, 0.04, 1.0);
    float ao = material.b * ssao;

    vec3 N = normalize(normal);
    vec3 V = normalize(u_ViewPos - fragPos);

    vec3 directLighting = vec3(0.0);

    if (u_HasDirectionalLight == 1) {
        vec3 L = normalize(-u_DirectionalLight.direction);
        vec3 radiance = u_DirectionalLight.color * u_DirectionalLight.intensity;
        vec3 irradiance_directionalLight = EvaluatePbrDirect(N, V, L, radiance, albedo, metallic, roughness);
        irradiance_directionalLight *= ComputeDirectionLightShadowFactor(fragPos);
        directLighting += irradiance_directionalLight;
    }

    for (int i = 0; i < u_PointLightCount; ++i) {
        PointLight light = u_PointLights[i];
        vec3 lightVector = light.position - fragPos;
        float distanceToLight = length(lightVector);
        float attenuation = ComputePointAttenuation(light, distanceToLight);
        if (attenuation <= 0.0) {
            continue;
        }

        vec3 L = distanceToLight > 0.0001 ? lightVector / distanceToLight : N;
        vec3 radiance = light.color * light.intensity * attenuation;
        directLighting += EvaluatePbrDirect(N, V, L, radiance, albedo, metallic, roughness);
    }

    float hemi = N.y * 0.5 + 0.5;
    vec3 skyColor = vec3(0.10, 0.12, 0.16);
    vec3 groundColor = vec3(0.13, 0.13, 0.13);
    vec3 hemiLight = mix(groundColor, skyColor, hemi);
    float abient_intensity = 3.0;
    vec3 ambient = abient_intensity * hemiLight * albedo * ao;

    vec3 radiance = ambient + directLighting + emissive;
    FragColor = vec4(radiance, 1.0);
}
