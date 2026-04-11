#version 410
layout(location=0) out float sDepth;

in vec4 v_lightSpacePos;

void main(){
    float depth = v_lightSpacePos.z/v_lightSpacePos.w * 0.5 + 0.5;
    sDepth = depth;
}
