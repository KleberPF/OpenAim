#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float textureScale = 1.0f;
uniform sampler2D diffuse;

void main()
{    
    FragColor = texture(diffuse, textureScale * TexCoords);
}