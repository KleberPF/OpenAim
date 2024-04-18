#pragma once

#include <glm/glm.hpp>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
class Camera {
public:
    Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
        float pitch);

    glm::mat4 getViewMatrix() const;
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    float getZoom() const;

    void setMouseSensitivity(float sensitivity);

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // euler angles
    float m_yaw;
    float m_pitch;

    // options
    float m_movementSpeed = 8.0f;
    float m_mouseSensitivity = 0.03;
    float m_zoom = 45.0;

    void updateVectors();
};