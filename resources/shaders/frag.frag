#version 450

layout(binding = 1) uniform sampler2D textSampler[];

layout(location = 0) in vec2 UV;
layout(location = 1) flat in int textureIndex;


layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(textSampler[0], UV);
}