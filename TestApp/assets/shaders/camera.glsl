#type vertex
#version 460 core

layout (location = 0) in vec3 a_Position;
out vec3 v_Position;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    v_Position = a_Position;
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core
layout (location = 0) out vec4 color;
in vec3 v_Position;

void main()
{
    color = vec4(v_Position, 1.0);
}