#pragma once
#include "glm.hpp"
#include <string>
glm::vec3 ParseVector(std::string line);
glm::quat ParseQuaternion(std::string line);
std::pair<std::string, std::string> GenerateKeyValuePair(std::string line, std::string delimiter);