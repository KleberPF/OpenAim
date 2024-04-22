#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 color;
    float textureScale;
};

uniform Material material;

void main()
{
    FragColor = vec4(material.color, 1.0) * texture(material.diffuse, material.textureScale * TexCoords);
}