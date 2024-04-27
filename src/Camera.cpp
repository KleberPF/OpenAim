#include "Camera.hpp"

#include "glm/geometric.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(
    const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
    : m_position(position)
    , m_worldUp(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
{
    updateVectors();
}

glm::mat4 Camera::buildViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;

    if (direction == CameraMovement::FORWARD
        || direction == CameraMovement::BACKWARD) {

        // unit vector normal to the xz plane
        glm::vec3 normal = glm::normalize(glm::cross(
            glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        // front vector projected onto the xz plane and normalized
        glm::vec3 projected
            = glm::normalize(m_front - (glm::dot(m_front, normal)) * normal);

        if (direction == CameraMovement::BACKWARD) {
            projected *= -1;
        }

        m_position += projected * velocity;
    } else if (direction == CameraMovement::LEFT) {
        m_position -= m_right * velocity;
    } else if (direction == CameraMovement::RIGHT) {
        m_position += m_right * velocity;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    // constrain pitch
    if (m_pitch > 89.0F) {
        m_pitch = 89.0F;
    } else if (m_pitch < -89.0F) {
        m_pitch = -89.0F;
    }

    updateVectors();
}

glm::vec3 Camera::position() const
{
    return m_position;
}

glm::vec3 Camera::front() const
{
    return m_front;
}

float Camera::zoom() const
{
    return m_zoom;
}

void Camera::setMouseSensitivity(float sensitivity)
{
    m_mouseSensitivity = sensitivity;
}

void Camera::updateVectors()
{
    // calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // also re-calculate the right and up vector
    m_right = glm::normalize(glm::cross(m_front,
        m_worldUp)); // normalize the vectors, because their length gets closer
                     // to 0 the more you look up or down which results in
                     // slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}