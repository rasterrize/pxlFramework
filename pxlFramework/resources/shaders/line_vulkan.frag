#version 450 core

layout (location = 0) out vec4 color;

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec4 v_Colour;

void main()
{
    color = v_Colour;
}