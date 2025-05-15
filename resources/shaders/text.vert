#version 450 core
in vec2 Position;
in vec2 TexCoords;

out vec2 Frag_TexCoords;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(Position.xy, 0.0, 1.0);
    Frag_TexCoords = TexCoords;
}