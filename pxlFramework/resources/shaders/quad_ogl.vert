#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Colour;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in float a_TexIndex;

out vec3 v_Position;
out vec4 v_Colour;

uniform mat4 u_VP;

void main()
{
    v_Position = a_Position;
    v_Colour = a_Colour;

    gl_Position = u_VP * vec4(a_Position, 1.0);
}