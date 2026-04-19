#version 450 core

layout(location = 0) in vec3 aPos;

out vec3 vTexCoord;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    vTexCoord = aPos;
    vec4 position = u_Projection * u_View * vec4(aPos, 1.0);
    gl_Position = position.xyww;
}
