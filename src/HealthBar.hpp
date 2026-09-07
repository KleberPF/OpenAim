#pragma once

#include "Color.hpp"
#include "Geometry.hpp"

class HealthBar {
public:
    HealthBar(Geometry geometry);

    const Geometry& geometry() const;
    void update(float healthPercentage);
    // Needs to be called before rendering
    void updateMaterial();

    void setPosition(const glm::vec3& position);
    void setSize(const glm::vec3& size);
    void setRotation(const glm::vec3& rotation);

private:
    Geometry m_geometry;
    float m_healthPercentage;
    Color m_color;
};
