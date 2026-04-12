#version 410 core

out vec4 FragColor;
in vec2 vTexCoords;

const int KERNEL_SIZE = 32;

uniform sampler2D u_GPosition;
uniform sampler2D u_GNormal;
uniform sampler2D u_NoiseTexture;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_Samples[KERNEL_SIZE];
uniform float u_Radius;
uniform float u_Bias;
uniform float u_NoiseScaleX;
uniform float u_NoiseScaleY;

void main() {
    vec3 encodedNormal = texture(u_GNormal, vTexCoords).rgb;
    if (dot(encodedNormal, encodedNormal) < 1e-6) {
        FragColor = vec4(1.0);
        return;
    }

    vec3 fragPosWorld = texture(u_GPosition, vTexCoords).rgb;
    vec3 normalWorld = normalize(encodedNormal * 2.0 - 1.0);

    vec3 fragPos = vec3(u_View * vec4(fragPosWorld, 1.0));
    vec3 normal = normalize(mat3(u_View) * normalWorld);

    vec2 noiseScale = vec2(u_NoiseScaleX, u_NoiseScaleY);
    vec3 randomVec = normalize(texture(u_NoiseTexture, vTexCoords * noiseScale).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; ++i) {
        vec3 samplePos = fragPos + TBN * u_Samples[i] * u_Radius;
        vec4 offset = u_Projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0) {
            continue;
        }

        vec3 sampleWorldPos = texture(u_GPosition, offset.xy).rgb;
        vec3 sampleViewPos = vec3(u_View * vec4(sampleWorldPos, 1.0));
        float sampleDepth = sampleViewPos.z;

        float rangeCheck = smoothstep(0.0, 1.0, u_Radius / max(abs(fragPos.z - sampleDepth), 0.0001));
        occlusion += (sampleDepth >= samplePos.z + u_Bias ? 1.0 : 0.0) * rangeCheck;
    }

    float ao = 1.0 - (occlusion / float(KERNEL_SIZE));
    FragColor = vec4(vec3(ao), 1.0);
}
