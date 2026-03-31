#version 410 core
in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D u_ScreenTexture;

void main(){
    FragColor = vec4(texture(u_ScreenTexture,vTexCoords).rgb,1.0);
}