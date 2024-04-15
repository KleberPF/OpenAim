#pragma once

#include "glm/glm.hpp"

// Generates the rotation matrix from the angles of rotation
// in each axis
glm::mat4 anglesToRotationMatrix(const glm::vec3& rotation);