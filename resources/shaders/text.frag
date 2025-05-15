#version 450 core
in vec2 Frag_TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, Frag_TexCoords).r);
    FragColor = vec4(textColor, 1.0) * sampled;
}