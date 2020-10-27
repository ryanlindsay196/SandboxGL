#include "MathHelperFunctions.h"
#include <algorithm>

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

glm::vec3 MathHelperFunctions::AngleAxisToEuler(glm::vec3 axis, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	float t = 1 - c;

	glm::vec3 returnEulers;

	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(x*x + y*y + z*z);
	// if (magnitude==0) throw error;
	// x /= magnitude;
	// y /= magnitude;
	// z /= magnitude;
	if ((axis.x*axis.y*t + axis.z * s) > 0.998) { // north pole singularity detected
		returnEulers.x = 2 * atan2(axis.x*sinf(angle / 2), cosf(angle / 2));
		returnEulers.y = M_PI / 2;
		returnEulers.z = 0;
		return returnEulers;
	}
	if ((axis.x*axis.y*t + axis.z * s) < -0.998) { // south pole singularity detected
		returnEulers.x = -2 * atan2(axis.x*sinf(angle / 2), cosf(angle / 2));
		returnEulers.y = -M_PI / 2;
		returnEulers.z = 0;
		return returnEulers;
	}
	returnEulers.x = atan2(axis.y * s - axis.x * axis.z * t, 1 - (axis.y*axis.y + axis.z * axis.z) * t);
	returnEulers.y = asin(axis.x * axis.y * t + axis.z * s);
	returnEulers.z = atan2(axis.x * s - axis.y * axis.z * t, 1 - (axis.x*axis.x + axis.z * axis.z) * t);
	return returnEulers;
}

AngleAxis MathHelperFunctions::EulerToAngleAxis(glm::vec3 eulers)
{
	//This function doesn't work
	assert(0);

	// Assuming the angles are in radians.
	float c1 = cosf(eulers.x / 2);
	float s1 = sinf(eulers.x / 2);
	float c2 = cosf(eulers.y / 2);
	float s2 = sinf(eulers.y / 2);
	float c3 = cosf(eulers.z / 2);
	float s3 = sinf(eulers.z / 2);
	float c1c2 = c1 * c2;
	float s1s2 = s1 * s2;
	AngleAxis angleAxis;

	float w = c1c2 * c3 - s1s2 * s3;
	angleAxis.axis.x = c1c2 * s3 + s1s2 * c3;
	angleAxis.axis.y = s1 * c2*c3 + c1 * s2*s3;
	angleAxis.axis.z = c1 * s2*c3 - s1 * c2*s3;
	angleAxis.angle = 2 * acos(w);
	double norm = angleAxis.axis.x * angleAxis.axis.x + angleAxis.axis.y * angleAxis.axis.y + angleAxis.axis.z * angleAxis.axis.z;
	if (norm < 0.0000001) { // when all euler angles are zero angle =0 so
		// we can set axis to anything to avoid divide by zero
		angleAxis.axis.x = 1;
		angleAxis.axis.y = angleAxis.axis.z = 0;
	}
	else {
		norm = sqrt(norm);
		angleAxis.axis.x /= norm;
		angleAxis.axis.y /= norm;
		angleAxis.axis.z /= norm;
	}

	std::swap(angleAxis.axis.z, angleAxis.axis.y);
	//angleAxis.axis.y *= -1;
	float axisDist = glm::length(angleAxis.axis);
	angleAxis.axis = angleAxis.axis / axisDist;
	return angleAxis;
}

