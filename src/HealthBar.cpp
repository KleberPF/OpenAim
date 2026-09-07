#include "HealthBar.hpp"

#include "Color.hpp"

HealthBar::HealthBar(Geometry geometry)
    : m_geometry(geometry)
{
}

const Geometry& HealthBar::geometry() const
{
    return m_geometry;
}

void HealthBar::update(float healthPercentage)
{
    auto getColor = [](float healthPercentage) {
        if (healthPercentage >= 0.75f) {
            return COLOR_GREEN;
        }
        if (healthPercentage >= 0.5f) {
            return Color { .r = 255, .g = 170, .b = 0 };
        }
        if (healthPercentage >= 0.25f) {
            return Color { .r = 255, .g = 85, .b = 0 };
        }

        return COLOR_RED;
    };

    m_healthPercentage = healthPercentage;
    m_color = getColor(healthPercentage);
}

void HealthBar::updateMaterial()
{
    const Shader* shader = m_geometry.m_shader;
    shader->use();

    shader->setFloat("healthPercentage", m_healthPercentage);

    Material* material = m_geometry.m_material;
    material->setColor(m_color.toOpenGLFormat());
    material->bind(*shader);
}

void HealthBar::setPosition(const glm::vec3& position)
{
    m_geometry.setPosition(position);
}

void HealthBar::setSize(const glm::vec3& size)
{
    m_geometry.setSize(size);
}

void HealthBar::setRotation(const glm::vec3& rotation)
{
    m_geometry.setRotation(rotation);
}
