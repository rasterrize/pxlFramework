#version 450 core
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec4 v_Colour;
layout (location = 2) in vec2 v_TexCoords;
layout (location = 3) flat in int v_TexIndex;

layout (location = 0) out vec4 color;

layout (set = 0, binding = 0) uniform sampler2D tex[];

void main()
{
    color = texture(tex[nonuniformEXT(v_TexIndex)], v_TexCoords) * v_Colour;
}