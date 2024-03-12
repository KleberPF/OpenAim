#pragma once

#include "GLObject.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>

#include <array>
#include <optional>
#include <vector>

using IntersectionResult = std::optional<glm::vec3>;

class BaseEntity
{
public:
    BaseEntity(std::vector<float> vertices,
               std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
               glm::vec4 color);

    BaseEntity(const BaseEntity& baseEntity) = default;
    BaseEntity& operator=(const BaseEntity& baseEntity) = default;

    BaseEntity(BaseEntity&& baseEntity) = default;
    BaseEntity& operator=(BaseEntity&& baseEntity) = default;
    virtual ~BaseEntity() = default;

    const Shader& getShader() const;
    const glm::vec3& getPos() const;
    const glm::vec4& getColor() const;

    virtual void onHit()
    {
    }

    /*
    * Returns the closest intersection between this entity and the line
    * described by eyePos and eyeDir
    */
    IntersectionResult isIntersectedByLine(const glm::vec3& eyePos,
                                           const glm::vec3& eyeDir);

    void move(const glm::vec3& newPos);

    void render();

private:
    Shader shader_;
    GLObject glObject_;
    std::array<glm::vec3, 2> aabb_;
    glm::vec3 pos_;
    glm::vec4 color_;

    bool isPointInPlaneSection(const glm::vec3& point);
};

class Obstacle : public BaseEntity
{
    void onHit() override;
};

class Target : public BaseEntity
{
public:
    Target(std::vector<float> vertices,
           std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
           glm::vec4 color);

    void onHit() override;

private:
    int health_;
};