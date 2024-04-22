#pragma once

#include <glm/glm.hpp>

// Returns RzRyRx, where Ra is the rotation matrix around the
// t axis
glm::mat4 anglesToRotationMatrix(const glm::vec3& rotation);

// Returns the inverse of RzRyRx, where Ra is the rotation
// matrix around the axis a
// glm::mat4 anglesToRotationMatrix(const glm::vec3& rotation);