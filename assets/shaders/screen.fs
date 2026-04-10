#version 410 core
in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D u_ScreenTexture;
uniform float hdr_exposure;

void main(){
    vec3 hdr_radiance = texture(u_ScreenTexture,vTexCoords).rgb;
    vec3 ldr_color = vec3(1.0) - exp(-hdr_radiance * hdr_exposure);
    vec3 tone_color = pow(ldr_color, vec3(1.0 / 2.2));
    FragColor = vec4(tone_color,1.0);
}