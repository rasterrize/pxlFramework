#version 450 core

layout (location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Colour;

void main()
{
    color = v_Colour;
}