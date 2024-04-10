#include "Camera.hpp"

#include "glm/ext/matrix_transform.hpp"

#include <iostream>

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
               float pitch)
    : position_(position)
    , worldUp_(up)
    , yaw_(yaw)
    , pitch_(pitch)
{
    this->updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(this->position_, this->position_ + this->front_,
                       this->up_);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = this->movementSpeed_ * deltaTime;

    switch (direction)
    {
        case CameraMovement::FORWARD:
            this->position_ += this->front_ * velocity;
            break;
        case CameraMovement::BACKWARD:
            this->position_ -= this->front_ * velocity;
            break;
        case CameraMovement::LEFT:
            this->position_ -= this->right_ * velocity;
            break;
        case CameraMovement::RIGHT:
            this->position_ += this->right_ * velocity;
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset)
{
    xoffset *= this->mouseSensitivity_;
    yoffset *= this->mouseSensitivity_;

    this->yaw_ += xoffset;
    this->pitch_ += yoffset;

    // constrain pitch
    if (this->pitch_ > 89.0F)
    {
        this->pitch_ = 89.0F;
    }
    else if (this->pitch_ < -89.0F)
    {
        this->pitch_ = -89.0F;
    }

    this->updateVectors();
}

glm::vec3 Camera::getPosition() const
{
    return this->position_;
}

glm::vec3 Camera::getFront() const
{
    return this->front_;
}

float Camera::getZoom() const
{
    return this->zoom_;
}

void Camera::updateVectors()
{
    // calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
    front.y = sin(glm::radians(this->pitch_));
    front.z = sin(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
    this->front_ = glm::normalize(front);
    // also re-calculate the right and up vector
    this->right_ = glm::normalize(glm::cross(
        this->front_,
        this->worldUp_));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->up_ = glm::normalize(glm::cross(this->right_, this->front_));
}