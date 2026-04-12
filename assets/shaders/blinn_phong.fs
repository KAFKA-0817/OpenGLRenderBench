#version 410 core

out vec4 FragColor;

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

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
    vec3 albedo;
    vec3 specular;
    float shininess;
    int hasDiffuseMap;
    int hasSpecularMap;
};

uniform int u_HasDirectionalLight;
uniform DirectionalLight u_DirectionalLight;
uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform MaterialData u_Material;
uniform vec3 u_ViewPos;
uniform mat4 u_LightSpaceMatrix;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_ShadowMap;

vec3 EvaluateBlinnPhong(vec3 N,
                        vec3 V,
                        vec3 L,
                        vec3 radiance,
                        vec3 albedo,
                        vec3 specularColor,
                        float shininess) {
    vec3 H = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    return diff * albedo * radiance + spec * specularColor * radiance;
}

float ComputePointAttenuation(PointLight light, float distanceToLight) {
    if (light.range <= 0.0001 || distanceToLight >= light.range) {
        return 0.0;
    }

    float normalizedDistance = clamp(distanceToLight / light.range, 0.0, 1.0);
    float falloff = 1.0 - normalizedDistance;
    return falloff * falloff;
}

float ComputeDirectionLightShadowFactor(vec3 fragPos) {
    vec4 lightSpacePos = u_LightSpaceMatrix * vec4(fragPos, 1.0);
    lightSpacePos /= lightSpacePos.w;
    lightSpacePos = lightSpacePos * 0.5 + 0.5;

    if (lightSpacePos.x < 0.0 || lightSpacePos.x > 1.0 ||
        lightSpacePos.y < 0.0 || lightSpacePos.y > 1.0 ||
        lightSpacePos.z < 0.0 || lightSpacePos.z > 1.0) {
        return 1.0;
    }

    float currentDepth = lightSpacePos.z;
    float sampleDepth = texture(u_ShadowMap, lightSpacePos.xy).r;
    return currentDepth <= sampleDepth + 0.005 ? 1.0 : 0.0;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(u_ViewPos - vFragPos);

    vec3 albedo = u_Material.albedo;
    if (u_Material.hasDiffuseMap == 1) {
        albedo *= texture(u_DiffuseMap, vTexCoord).rgb;
    }

    vec3 specularColor = u_Material.specular;
    if (u_Material.hasSpecularMap == 1) {
        specularColor *= texture(u_SpecularMap, vTexCoord).rgb;
    }

    vec3 directLighting = vec3(0.0);

    if (u_HasDirectionalLight == 1) {
        vec3 L = normalize(-u_DirectionalLight.direction);
        vec3 radiance = u_DirectionalLight.color * u_DirectionalLight.intensity;
        vec3 directionalLighting = EvaluateBlinnPhong(N, V, L, radiance, albedo, specularColor, u_Material.shininess);
        directionalLighting *= ComputeDirectionLightShadowFactor(vFragPos);
        directLighting += directionalLighting;
    }

    for (int i = 0; i < u_PointLightCount; ++i) {
        PointLight light = u_PointLights[i];
        vec3 lightVector = light.position - vFragPos;
        float distanceToLight = length(lightVector);
        float attenuation = ComputePointAttenuation(light, distanceToLight);
        if (attenuation <= 0.0) {
            continue;
        }

        vec3 L = distanceToLight > 0.0001 ? lightVector / distanceToLight : N;
        vec3 radiance = light.color * light.intensity * attenuation;
        directLighting += EvaluateBlinnPhong(N, V, L, radiance, albedo, specularColor, u_Material.shininess);
    }

    vec3 ambient = 0.05 * albedo;
    vec3 color = ambient + directLighting;
    FragColor = vec4(color, 1.0);
}
