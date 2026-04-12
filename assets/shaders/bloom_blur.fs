#version 410 core

in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D u_Image;
uniform int u_Horizontal;

void main() {
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec2 texel_size = 1.0 / vec2(textureSize(u_Image, 0));

    vec3 result = texture(u_Image, vTexCoords).rgb * weights[0];
    for (int i = 1; i < 5; ++i) {
        vec2 offset = u_Horizontal == 1
                          ? vec2(texel_size.x * float(i), 0.0)
                          : vec2(0.0, texel_size.y * float(i));
        result += texture(u_Image, vTexCoords + offset).rgb * weights[i];
        result += texture(u_Image, vTexCoords - offset).rgb * weights[i];
    }

    FragColor = vec4(result, 1.0);
}
