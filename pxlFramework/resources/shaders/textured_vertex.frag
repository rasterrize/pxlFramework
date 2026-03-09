#version 450 core

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec4 v_Colour;
layout (location = 2) in vec2 v_TexCoords;
layout (location = 3) in float v_TexIndex;

layout (location = 0) out vec4 color;

void main()
{
    color = v_Colour;
}