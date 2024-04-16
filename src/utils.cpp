#include "utils.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <array>

glm::mat4 anglesToRotationMatrix(const glm::vec3& rotation)
{
    auto model = glm::identity<glm::mat4>();
    const std::array<glm::vec3, 3> rotationAxis = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1),
    };
    for (int i = 2; i >= 0; i--) {
        if (rotation[i] != 0) {
            model = glm::rotate(
                model, glm::radians(rotation[i]), rotationAxis[i]);
        }
    }

    return model;
}