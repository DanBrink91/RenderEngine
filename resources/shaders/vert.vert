#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_draw_parameters : enable

struct SpriteVertex {
    vec2 position;
    vec2 uv;
};

struct SpriteDrawData
{
   int textureIndex;
   vec3 glowColor;
};

layout(set = 0, binding = 0) readonly buffer SpriteVertices {
    SpriteVertex spriteVertex[];
};

layout(set = 0, binding = 1) readonly buffer SpritesDrawData {
    SpriteDrawData spriteDrawData[];
};

layout(location = 0) out vec2 UV;
layout(location = 1) out int textureIndex;

void main() {
    SpriteVertex sprite = spriteVertex[gl_VertexIndex];
    SpriteDrawData dd = spriteDrawData[gl_VertexIndex / 4];
    
    vec2 worldPos = 2 * UV - 0.5;
    vec2 offset = 0.001 * normalize(worldPos);

    gl_Position = vec4(sprite.position, 0.0, 1.0);
    UV = sprite.uv;
    textureIndex = dd.textureIndex;
}