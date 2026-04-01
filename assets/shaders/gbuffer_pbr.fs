#version 410 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gMaterial;
layout(location = 4) out vec4 gEmissive;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

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

uniform sampler2D u_BaseColorMap;
uniform sampler2D u_MetallicRoughnessMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_OcclusionMap;
uniform sampler2D u_EmissiveMap;

void main() {
    vec3 baseColor = u_Material.baseColorFactor;
    float alpha = u_Material.alphaFactor;

    if (u_Material.hasBaseColorMap == 1) {
        vec4 baseSample = texture(u_BaseColorMap, fs_in.TexCoord);
        baseColor *= baseSample.rgb;
        alpha *= baseSample.a;
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

    vec3 N = normalize(fs_in.Normal);
    // 这里先保留 normal map 接口，但当前没有 TBN，暂不实际应用扰动
    // 以后补 tangent-space normal mapping 时再接入 u_NormalMap + u_Material.normalScale

    gPosition = vec4(fs_in.FragPos, 1.0);
    gNormal = vec4(N * 0.5 + 0.5, 1.0);
    gAlbedo = vec4(baseColor, alpha);
    gMaterial = vec4(metallic, roughness, ao, 1.0);
    gEmissive = vec4(emissive, 1.0);
}