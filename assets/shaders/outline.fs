#version 410
in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D u_MaskTexture;
uniform sampler2D u_SceneTexture;
uniform float u_TexelWidth;
uniform float u_TexelHeight;
uniform int u_OutlineRadius;
uniform vec3 u_OutlineColor;

float sampleMask(vec2 uv) {
    return step(0.5, texture(u_MaskTexture, uv).r);
}

float computeOutline(vec2 uv) {
    float center = sampleMask(uv);
    if (center > 0.5) {
        return 0.0;
    }

    float neighbor_max = 0.0;
    for (int y = -u_OutlineRadius; y <= u_OutlineRadius; ++y) {
        for (int x = -u_OutlineRadius; x <= u_OutlineRadius; ++x) {
            if (x == 0 && y == 0) {
                continue;
            }

            vec2 offset = vec2(float(x) * u_TexelWidth, float(y) * u_TexelHeight);
            neighbor_max = max(neighbor_max, sampleMask(uv + offset));
        }
    }

    return neighbor_max;
}

void main() {
    vec4 scene = texture(u_SceneTexture, vTexCoords);
    float outline = computeOutline(vTexCoords);
    vec3 color = mix(scene.rgb, u_OutlineColor, outline);
    FragColor = vec4(color, scene.a);
}
