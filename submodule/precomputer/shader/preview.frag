#version 450 core

in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uBrdfLut;

void main() {
    vec2 brdf = texture(uBrdfLut, vTexCoord).rg;
    FragColor = vec4(brdf.r, brdf.g, 0.0, 1.0);
}
