#include "GLObject.hpp"

#include <cassert>
#include <cstdlib>

GLObject::GLObject(std::vector<float>& vertices,
                   std::optional<std::vector<GLuint>>& indices)
    : vertices_(std::move(vertices))
    , indices_(std::move(indices))
{
    assert(!this->vertices_.empty());
    auto a = this->vertices_.size();

    glGenVertexArrays(1, &this->vao_);
    glGenBuffers(1, &this->vbo_);
    // TODO: check if it's necessary to do this before glBindVertexArray
    if (this->indices_.has_value())
    {
        glGenBuffers(1, &this->ebo_);
    }
    glBindVertexArray(this->vao_);

    if (this->indices_.has_value())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(this->indices_.value()[0]) * this->indices_.value().size(),
            this->indices_.value().data(), GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(this->vertices_[0]) * this->vertices_.size(),
                 this->vertices_.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(this->vertices_[0]) * 3, (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLObject::~GLObject()
{
    glDeleteVertexArrays(1, &this->vao_);
    glDeleteBuffers(1, &this->vbo_);
    // TODO: check if it's fine to delete a possibly not created buffer
    glDeleteBuffers(1, &this->ebo_);
}

void GLObject::render()
{
    // shader.use is done in game render for now at least
    glBindVertexArray(this->vao_);
    if (this->indices_.has_value())
    {
        glDrawElements(GL_TRIANGLES, this->indices_.value().size(),
                       GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, this->vertices_.size() / 3);
    }
}