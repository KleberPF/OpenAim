#pragma once

#include <glm/glm.hpp>

#include <string>

// Returns RzRyRx, where Ra is the rotation matrix around the
// t axis
glm::mat4 anglesToRotationMatrix(const glm::vec3& rotation);

bool caseInsensitiveEquals(const std::string& str1, const std::string& str2);

// if the string only contains one number, fills out the vec3 with that number
glm::vec3 readVec3FromJSONString(const std::string& str);
