#include "utils.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <sstream>

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

bool caseInsensitiveEquals(const std::string& str1, const std::string& str2)
{
    auto cmp = [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a))
            == std::tolower(static_cast<unsigned char>(b));
    };

    return std::equal(str1.begin(), str1.end(), str2.begin(), str2.end(), cmp);
}

glm::vec3 readVec3FromJSONString(const std::string& str)
{
    auto ss = std::istringstream(str);
    auto result = glm::vec3(0.0f);
    // this starts at -1 because the ss tries to read one more time after the
    // whole string ends
    int index = -1;

    while (index < 2 && ss) {
        ss >> result[++index];
    }

    if (index == 1) {
        return glm::vec3(result.x);
    }

    return result;
}
