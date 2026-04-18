#version 450 core

in vec2 vTexCoord;

layout(location = 0) out vec2 FragColor;

void main() {
    FragColor = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
}
