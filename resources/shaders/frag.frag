#version 450
#extension GL_EXT_nonuniform_qualifier : enable


layout(binding = 2) uniform sampler2D textSampler[];

layout(location = 0) in vec2 UV;
layout(location = 1) flat in int textureIndex;



layout(location = 0) out vec4 outColor;


void main() {
    vec4 texColor = texture(textSampler[textureIndex], UV);
    outColor = texColor;
}