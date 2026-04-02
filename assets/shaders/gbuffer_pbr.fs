#version 410 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gMaterial;
layout(location = 4) out vec4 gEmissive;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 T;
    vec3 B;
    vec3 N;
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

    int alphaMode;      // 0=OPAQUE, 1=MASK, 2=BLEND
    float alphaCutoff;
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

    if (u_Material.alphaMode == 1 && alpha < u_Material.alphaCutoff) {
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

        vec3 T = normalize(fs_in.T);
        vec3 B = normalize(fs_in.B);
        vec3 NN = normalize(fs_in.N);
        mat3 TBN = mat3(T, B, NN);

        N = normalize(TBN * tangentNormal);
    }

    gPosition = vec4(fs_in.FragPos, 1.0);
    gNormal = vec4(N * 0.5 + 0.5, 1.0);
    gAlbedo = vec4(baseColor, alpha);
    gMaterial = vec4(metallic, roughness, ao, 1.0);
    gEmissive = vec4(emissive, 1.0);
}