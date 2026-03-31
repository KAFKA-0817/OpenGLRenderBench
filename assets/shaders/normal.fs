#version 410 core

in vec3 vNormal;
out vec4 FragColor;

void main() {
    vec3 n = normalize(vNormal);
    vec3 color = n * 0.5 + 0.5; // [-1,1] -> [0,1]
    FragColor = vec4(color, 1.0);
}