#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 mvp;

void main()
{
    FragPos = aPos;
    gl_Position = mvp * vec4(aPos, 1.0);
}