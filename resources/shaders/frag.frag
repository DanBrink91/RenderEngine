#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 2) uniform sampler2D textSampler[];

layout(location = 0) in vec2 UV;
layout(location = 1) flat in int textureIndex;
layout(location = 2) in float time;

layout(location = 0) out vec4 outColor;


void main() {
    vec4 outlineColor = vec4(1.0, 1.0, 0, 1.0);
    vec4 texColor = texture(textSampler[textureIndex], UV);
    if(texColor.a == 0)
    {
        vec2 jitter = vec2(cos(UV.y * 5.0 + time * 4.0) * 0.1, cos(UV.x * 5.0 + time * 4.0) * 0.1);
        if( texture(textSampler[textureIndex], UV + vec2(0, jitter.y)).a > 0 || 
            texture(textSampler[textureIndex], UV + vec2(0, -jitter.y)).a > 0 ||
            texture(textSampler[textureIndex], UV + vec2(jitter.x, 0)).a > 0 ||
            texture(textSampler[textureIndex], UV + vec2(-jitter.x, 0)).a > 0 ||
            // Diagonals
            texture(textSampler[textureIndex], UV + vec2(-jitter.x, jitter.y)).a > 0 ||
            texture(textSampler[textureIndex], UV + vec2(jitter.x, jitter.y)).a > 0 ||
            texture(textSampler[textureIndex], UV + vec2(-jitter.x, -jitter.y)).a > 0 ||
            texture(textSampler[textureIndex], UV + vec2(jitter.x, -jitter.y)).a > 0
            )
        {
            texColor = outlineColor;
        }
    }

    outColor = texColor;
}