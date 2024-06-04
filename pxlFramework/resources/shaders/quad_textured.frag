#version 450 core

layout (location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Colour;
in vec2 v_TexCoords;
in float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
    color = texture(u_Textures[int(v_TexIndex)], v_TexCoords) * vec4(v_Colour.rgb, 1.0);
}