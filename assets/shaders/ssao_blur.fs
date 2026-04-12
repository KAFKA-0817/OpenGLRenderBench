#version 410 core

out vec4 FragColor;
in vec2 vTexCoords;

uniform sampler2D u_InputTexture;
uniform float u_TexelSizeX;
uniform float u_TexelSizeY;

void main() {
    float result = 0.0;
    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            vec2 offset = vec2(float(x) * u_TexelSizeX, float(y) * u_TexelSizeY);
            result += texture(u_InputTexture, vTexCoords + offset).r;
        }
    }

    result /= 9.0;
    FragColor = vec4(vec3(result), 1.0);
}
