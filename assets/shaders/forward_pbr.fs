#version 410 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 T;
    vec3 B;
    vec3 N;
} fs_in;

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

struct MaterialData {
    vec3 baseColorFactor;
    float alphaFactor;

    float metallicFactor;
    float roughnessFactor;
    float normalScale;
    float occlusionStrength;

    vec3 emissiveFactor;
    int hasBaseColorMap;
    int hasMetallicRoughnessMap;
    int hasNormalMap;
    int hasOcclusionMap;
    int hasEmissiveMap;
};

uniform MaterialData u_Material;
uniform int u_HasDirectionalLight;
uniform DirectionalLight u_DirectionalLight;
uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform vec3 u_ViewPos;

uniform sampler2D u_BaseColorMap;
uniform sampler2D u_MetallicRoughnessMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_OcclusionMap;
uniform sampler2D u_EmissiveMap;
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLut;
uniform float u_PrefilterMaxLod;

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

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

vec3 EvaluatePbrIbl(vec3 N,
                    vec3 V,
                    vec3 albedo,
                    float metallic,
                    float roughness,
                    float ao) {
    float NdotV = max(dot(N, V), 0.0);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = FresnelSchlickRoughness(NdotV, F0, roughness);

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 irradiance = texture(u_IrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-V, N);
    vec3 prefilteredColor = textureLod(u_PrefilterMap, R, roughness * u_PrefilterMaxLod).rgb;
    vec2 brdf = texture(u_BrdfLut, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    return (kD * diffuse + specular) * ao;
}

float ComputePointAttenuation(PointLight light, float distanceToLight) {
    if (light.range <= 0.0001 || distanceToLight >= light.range) {
        return 0.0;
    }

    float normalizedDistance = clamp(distanceToLight / light.range, 0.0, 1.0);
    float falloff = 1.0 - normalizedDistance;
    return falloff * falloff;
}

void main() {
    vec3 baseColor = u_Material.baseColorFactor;
    float alpha = u_Material.alphaFactor;

    if (u_Material.hasBaseColorMap == 1) {
        vec4 baseSample = texture(u_BaseColorMap, fs_in.TexCoord);
        baseColor *= baseSample.rgb;
        alpha *= baseSample.a;
    }

    if (alpha <= 0.001) {
        discard;
    }

    float metallic = u_Material.metallicFactor;
    float roughness = u_Material.roughnessFactor;

    if (u_Material.hasMetallicRoughnessMap == 1) {
        vec4 mr = texture(u_MetallicRoughnessMap, fs_in.TexCoord);
        roughness *= mr.g;
        metallic *= mr.b;
    }

    float ao = 1.0;
    if (u_Material.hasOcclusionMap == 1) {
        ao = mix(1.0, texture(u_OcclusionMap, fs_in.TexCoord).r, u_Material.occlusionStrength);
    }

    vec3 emissive = u_Material.emissiveFactor;
    if (u_Material.hasEmissiveMap == 1) {
        emissive *= texture(u_EmissiveMap, fs_in.TexCoord).rgb;
    }

    vec3 N = normalize(fs_in.N);
    if (u_Material.hasNormalMap == 1) {
        vec3 tangentNormal = texture(u_NormalMap, fs_in.TexCoord).xyz * 2.0 - 1.0;
        tangentNormal.xy *= u_Material.normalScale;

        float tangentLen = length(fs_in.T);
        float bitangentLen = length(fs_in.B);
        if (tangentLen > 1e-5 && bitangentLen > 1e-5) {
            vec3 T = normalize(fs_in.T);
            vec3 B = normalize(fs_in.B);
            vec3 NN = normalize(fs_in.N);
            mat3 TBN = mat3(T, B, NN);
            N = normalize(TBN * tangentNormal);
        }
    }

    roughness = clamp(roughness, 0.04, 1.0);

    vec3 V = normalize(u_ViewPos - fs_in.FragPos);
    vec3 directLighting = vec3(0.0);

    if (u_HasDirectionalLight == 1) {
        vec3 L = normalize(-u_DirectionalLight.direction);
        vec3 radiance = u_DirectionalLight.color * u_DirectionalLight.intensity;
        directLighting += EvaluatePbrDirect(N, V, L, radiance, baseColor, metallic, roughness);
    }

    for (int i = 0; i < u_PointLightCount; ++i) {
        PointLight light = u_PointLights[i];
        vec3 lightVector = light.position - fs_in.FragPos;
        float distanceToLight = length(lightVector);
        float attenuation = ComputePointAttenuation(light, distanceToLight);
        if (attenuation <= 0.0) {
            continue;
        }

        vec3 L = distanceToLight > 0.0001 ? lightVector / distanceToLight : N;
        vec3 radiance = light.color * light.intensity * attenuation;
        directLighting += EvaluatePbrDirect(N, V, L, radiance, baseColor, metallic, roughness);
    }

    vec3 ambient = EvaluatePbrIbl(N, V, baseColor, metallic, roughness, ao);

    vec3 radiance = ambient + directLighting + emissive;
    FragColor = vec4(radiance, alpha);
}
