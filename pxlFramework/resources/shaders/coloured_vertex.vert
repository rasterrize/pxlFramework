#version 450 core
#extension GL_EXT_buffer_reference : require

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Colour;

layout (location = 0) out vec3 v_Position;
layout (location = 1) out vec4 v_Colour;

layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer UniformBuffer
{
    mat4 vp;
};

layout(std430, push_constant) uniform Uniforms {
    UniformBuffer bufferPtr;
} ubo;

void main()
{
    v_Position = a_Position;
    v_Colour = a_Colour;

    gl_Position = ubo.bufferPtr.vp * vec4(a_Position, 1.0);
}