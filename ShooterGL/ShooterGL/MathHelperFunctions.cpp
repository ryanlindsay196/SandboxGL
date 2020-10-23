#include "MathHelperFunctions.h"

glm::vec3 MathHelperFunctions::Rotation(glm::quat rotationQuat)
{
	glm::vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (rotationQuat.w * rotationQuat.x + rotationQuat.y * rotationQuat.z);
	double cosr_cosp = +1.0 - 2.0 * (rotationQuat.x * rotationQuat.x + rotationQuat.y * rotationQuat.y);
	angles.x = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (rotationQuat.w * rotationQuat.y - rotationQuat.z * rotationQuat.x);
	if (fabs(sinp) >= 1)
		angles.y = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		angles.y = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (rotationQuat.w * rotationQuat.z + rotationQuat.x * rotationQuat.y);
	double cosy_cosp = +1.0 - 2.0 * (rotationQuat.y * rotationQuat.y + rotationQuat.z * rotationQuat.z);
	angles.z = atan2(siny_cosp, cosy_cosp);

	return angles;
}

