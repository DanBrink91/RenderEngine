#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_draw_parameters : enable

struct SpriteVertex {
    vec2 position;
    vec2 uv;
};

layout(set = 0, binding = 0) readonly buffer SpriteVertices {
    SpriteVertex spriteVertex[];
};

layout(location = 0) out vec3 fragColor;


void main() {
    SpriteVertex sprite = spriteVertex[gl_VertexIndex];
    gl_Position = vec4(sprite.position, 0.0, 1.0);
    fragColor = vec3(1.0, 0.0,  0.0);
}