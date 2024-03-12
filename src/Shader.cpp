#include "Shader.hpp"

#include "filereader.hpp"

#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : id_(glCreateProgram())
{
    std::string vertexCode = readTextFile(vertexPath);
    std::string fragmentCode = readTextFile(fragmentPath);

    const char* vertexCodeCStr = vertexCode.c_str();
    const char* fragmentCodeCStr = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCodeCStr, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCodeCStr, nullptr);
    glCompileShader(fragmentShader);

    glAttachShader(this->id_, vertexShader);
    glAttachShader(this->id_, fragmentShader);
    glLinkProgram(this->id_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(this->id_);
}

void Shader::use() const
{
    glUseProgram(this->id_);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(this->id_, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(this->id_, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(this->id_, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(this->id_, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->id_, name.c_str()), 1,
                       GL_FALSE, &value[0][0]);
}
