#pragma once

#include <glm/glm.hpp>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
class Camera
{
public:
    Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
           float pitch);

    glm::mat4 getViewMatrix() const;
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    float getZoom() const;

private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;

    // euler angles
    float yaw_;
    float pitch_;

    // options
    float movementSpeed_ = 8.0f;
    float mouseSensitivity_ = 0.03;
    float zoom_ = 45.0;

    void updateVectors();
};