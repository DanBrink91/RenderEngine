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

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    SpriteVertex sprite = spriteVertex[gl_VertexIndex];
    gl_Position = vec4(sprite.position, 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}