#version 450 core

layout (location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Colour;
in vec2 v_TexCoords;
in float v_TexIndex;

uniform sampler2D u_Textures[gl_MaxTextureImageUnits];

void main()
{
    /*
        Accessing u_Textures with v_TexIndex directly is non-standard behaviour.
        It just happens to work properly on NVIDIA drivers but not AMD
    */
    switch (int(v_TexIndex))
    {
        case 0: color = texture(u_Textures[0], v_TexCoords) * v_Colour; break;
        case 1: color = texture(u_Textures[1], v_TexCoords) * v_Colour; break;
        case 2: color = texture(u_Textures[2], v_TexCoords) * v_Colour; break;
        case 3: color = texture(u_Textures[3], v_TexCoords) * v_Colour; break;
        case 4: color = texture(u_Textures[4], v_TexCoords) * v_Colour; break;
        case 5: color = texture(u_Textures[5], v_TexCoords) * v_Colour; break;
        case 6: color = texture(u_Textures[6], v_TexCoords) * v_Colour; break;
        case 7: color = texture(u_Textures[7], v_TexCoords) * v_Colour; break;
        case 8: color = texture(u_Textures[8], v_TexCoords) * v_Colour; break;
        case 9: color = texture(u_Textures[9], v_TexCoords) * v_Colour; break;
        case 10: color = texture(u_Textures[10], v_TexCoords) * v_Colour; break;
        case 11: color = texture(u_Textures[11], v_TexCoords) * v_Colour; break;
        case 12: color = texture(u_Textures[12], v_TexCoords) * v_Colour; break;
        case 13: color = texture(u_Textures[13], v_TexCoords) * v_Colour; break;
        case 14: color = texture(u_Textures[14], v_TexCoords) * v_Colour; break;
        case 15: color = texture(u_Textures[15], v_TexCoords) * v_Colour; break;
        case 16: color = texture(u_Textures[16], v_TexCoords) * v_Colour; break;
        case 17: color = texture(u_Textures[17], v_TexCoords) * v_Colour; break;
        case 18: color = texture(u_Textures[18], v_TexCoords) * v_Colour; break;
        case 19: color = texture(u_Textures[19], v_TexCoords) * v_Colour; break;
        case 20: color = texture(u_Textures[20], v_TexCoords) * v_Colour; break;
        case 21: color = texture(u_Textures[21], v_TexCoords) * v_Colour; break;
        case 22: color = texture(u_Textures[22], v_TexCoords) * v_Colour; break;
        case 23: color = texture(u_Textures[23], v_TexCoords) * v_Colour; break;
        case 24: color = texture(u_Textures[24], v_TexCoords) * v_Colour; break;
        case 25: color = texture(u_Textures[25], v_TexCoords) * v_Colour; break;
        case 26: color = texture(u_Textures[26], v_TexCoords) * v_Colour; break;
        case 27: color = texture(u_Textures[27], v_TexCoords) * v_Colour; break;
        case 28: color = texture(u_Textures[28], v_TexCoords) * v_Colour; break;
        case 29: color = texture(u_Textures[29], v_TexCoords) * v_Colour; break;
        case 30: color = texture(u_Textures[30], v_TexCoords) * v_Colour; break;
        case 31: color = texture(u_Textures[31], v_TexCoords) * v_Colour; break;
    }
}