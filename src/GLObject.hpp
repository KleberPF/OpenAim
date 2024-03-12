#pragma once

#include <glad/glad.h>

#include <optional>
#include <vector>

class GLObject
{
public:
    GLObject(std::vector<float>& vertices,
             std::optional<std::vector<GLuint>>& indices);
    ~GLObject();

    GLObject(const GLObject& GLObject) = default;
    GLObject& operator=(const GLObject& GLObject) = default;

    GLObject(GLObject&& GLObject) = default;
    GLObject& operator=(GLObject&& GLObject) = default;

    void render();

private:
    std::vector<float> vertices_;
    std::optional<std::vector<GLuint>> indices_;
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
};