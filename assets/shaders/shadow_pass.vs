#version 410 core

layout(location = 0) in vec3 aPos;

out vec4 v_lightSpacePos;

uniform mat4 u_Model;
uniform mat4 u_lightSpaceMatrix;

void main(){
    v_lightSpacePos = u_lightSpaceMatrix * u_Model * vec4(aPos,1.0);
    gl_Position = v_lightSpacePos;
}