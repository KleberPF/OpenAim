#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = projection * model * vec4(aPos, 0.0f, 1.0f);
}