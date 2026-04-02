#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 T;
    vec3 B;
    vec3 N;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.TexCoord = aTexCoord;

    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    vs_out.T = normalize(normalMatrix * aTangent);
    vs_out.B = normalize(normalMatrix * aBitangent);
    vs_out.N = normalize(normalMatrix * aNormal);

    gl_Position = u_Projection * u_View * worldPos;
}