#include "Collider.h"
#include "RigidBody.h"

void Collider::Update(float gameTime)
{
}

//Calculates and returns the projections of this rigidbody on the x, y, and z axes.
Collider::ColliderProjections Collider::CalculateProjections(bool addVelocity, bool addStoredVelocity, float gameTime)
{
	ColliderProjections projections = ColliderProjections();

	glm::vec3 colliderCenter = colliderParent->componentParent->GetTranslation() + positionOffset;

	glm::vec3 velocity = colliderParent->GetVelocity();
	glm::vec3 storedVelocity = colliderParent->GetStoredVelocity();

	projections.x[0] = glm::dot(colliderCenter, glm::vec3(1, 0, 0)) + (velocity.x * addVelocity * gameTime) + (storedVelocity.x * addStoredVelocity * gameTime) - scale.x;
	projections.x[1] = glm::dot(colliderCenter, glm::vec3(1, 0, 0)) + (velocity.x * addVelocity * gameTime) + (storedVelocity.x * addStoredVelocity * gameTime) + scale.x;
	projections.y[0] = glm::dot(colliderCenter, glm::vec3(0, 1, 0)) + (velocity.y * addVelocity * gameTime) + (storedVelocity.y * addStoredVelocity * gameTime) - scale.y;
	projections.y[1] = glm::dot(colliderCenter, glm::vec3(0, 1, 0)) + (velocity.y * addVelocity * gameTime) + (storedVelocity.y * addStoredVelocity * gameTime) + scale.y;
	projections.z[0] = glm::dot(colliderCenter, glm::vec3(0, 0, 1)) + (velocity.z * addVelocity * gameTime) + (storedVelocity.z * addStoredVelocity * gameTime) - scale.z;
	projections.z[1] = glm::dot(colliderCenter, glm::vec3(0, 0, 1)) + (velocity.z * addVelocity * gameTime) + (storedVelocity.z * addStoredVelocity * gameTime) + scale.z;

	return projections;
}