#version 410 core

in vec3 vFragPos;
out vec4 FragColor;

void main() {
    vec3 color = vFragPos * 0.1 + 0.5;
    FragColor = vec4(color, 1.0);
}