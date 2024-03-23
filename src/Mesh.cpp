#include "Mesh.hpp"

#include <utility>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
           std::vector<Texture>& textures)
    : vertices(std::move(vertices))
    , indices(std::move(indices))
    , textures(std::move(textures))
{
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
                 this->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
                 this->indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, bitangent));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex),
                           (void*)offsetof(Vertex, boneIds));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, weights));
    glBindVertexArray(0);
}

void Mesh::render(const Shader& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < this->textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = this->textures[i].type;
        if (name == "textureDiffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "textureSpecular")
        {
            number = std::to_string(specularNr++);
        }
        else if (name == "textureNormal")
        {
            number = std::to_string(normalNr++);
        }
        else if (name == "textureHeight")
        {
            number = std::to_string(heightNr++);
        }

        shader.setInt(name + number, i);
        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLuint>(this->indices.size()),
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
