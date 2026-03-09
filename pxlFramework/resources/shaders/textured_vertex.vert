#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Colour;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in float a_TexIndex;

layout (location = 0) out vec3 v_Position;
layout (location = 1) out vec4 v_Colour;
layout (location = 2) out vec2 v_TexCoords;
layout (location = 3) out float v_TexIndex;

void main()
{
    v_Position = a_Position;
    v_Colour = a_Colour;

    gl_Position = vec4(a_Position, 1.0);
}