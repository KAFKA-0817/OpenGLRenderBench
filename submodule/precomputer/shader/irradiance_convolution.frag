#version 450 core

in vec3 vLocalPos;
layout(location = 0) out vec4 FragColor;

uniform samplerCube uEnvironmentMap;

const float PI = 3.14159265359;

void main() {
    vec3 normal = normalize(vLocalPos);
    vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    vec3 irradiance = vec3(0.0);
    float sample_count = 0.0;
    const float sample_delta = 0.025;

    for (float phi = 0.0; phi < 2.0 * PI; phi += sample_delta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sample_delta) {
            vec3 tangent_sample = vec3(
                sin(theta) * cos(phi),
                sin(theta) * sin(phi),
                cos(theta)
            );
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * normal;
            irradiance += texture(uEnvironmentMap, sample_vec).rgb * cos(theta) * sin(theta);
            sample_count += 1.0;
        }
    }

    irradiance = PI * irradiance / max(sample_count, 1.0);
    FragColor = vec4(irradiance, 1.0);
}
