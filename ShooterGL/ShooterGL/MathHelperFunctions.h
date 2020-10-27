#pragma once
#include "glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"

#define M_PI   3.14159265358979323846264338327950288

struct AngleAxis
{
	float angle;
	glm::vec3 axis;
};

class MathHelperFunctions
{
public:
	static glm::vec3 Rotation(glm::quat rotationQuat);
	static glm::vec3 AngleAxisToEuler(glm::vec3 axis, float angle);
	static AngleAxis EulerToAngleAxis(glm::vec3 eulers);
};

