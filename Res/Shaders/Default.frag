#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(location = 2) in vec2 coord;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main() {
    outColor = texture(texSampler, coord);
}
