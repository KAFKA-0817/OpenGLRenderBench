#version 450 core

in vec3 vLocalPos;
layout(location = 0) out vec4 FragColor;

uniform sampler2D uEquirectangularMap;

vec2 sampleSphericalMap(vec3 direction) {
    const vec2 inv_atan = vec2(0.1591549431, 0.3183098862);
    vec2 uv = vec2(atan(direction.z, direction.x), asin(clamp(direction.y, -1.0, 1.0)));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main() {
    vec3 direction = normalize(vLocalPos);
    vec2 uv = sampleSphericalMap(direction);
    vec3 color = texture(uEquirectangularMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
