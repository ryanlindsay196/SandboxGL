#pragma once
#include "glm.hpp"
#include <string>
glm::vec3 ParseVector(std::string line);
std::pair<std::string, std::string> GenerateKeyValuePair(std::string line, std::string delimiter);