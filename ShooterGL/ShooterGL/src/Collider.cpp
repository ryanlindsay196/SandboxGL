#include "Collider.h"
#include "RigidBody.h"
#include "Debug/DrawRay.h"

#ifdef DEBUG
void Collider::CreateDebugLines()
{
	DebugLines* debugLines = DebugLines::GetInstance();
	glm::vec3 colliderCenter = /*colliderParent->componentParent->GetTranslation() + */positionOffset;
	//Lines that span the z-axis
	debugLines->AddLine(colliderCenter + glm::vec3(-scale.x, -scale.y, -scale.z),
		colliderCenter + glm::vec3(-scale.x, -scale.y, scale.z),
		colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, -scale.y, -scale.z),
		colliderCenter + glm::vec3(scale.x, -scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(-scale.x, scale.y, -scale.z),
		colliderCenter + glm::vec3(-scale.x, scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, scale.y, -scale.z),
		colliderCenter + glm::vec3(scale.x, scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
	//lines that span the y-axis
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, -scale.y, -scale.z),
		colliderCenter + glm::vec3(scale.x, scale.y, -scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, -scale.y, scale.z),
		colliderCenter + glm::vec3(scale.x, scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(-scale.x, scale.y, scale.z),
		colliderCenter + glm::vec3(-scale.x, -scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(-scale.x, scale.y, -scale.z),
		colliderCenter + glm::vec3(-scale.x, -scale.y, -scale.z),
	colliderParent->componentParent, debugLinesColor);
	//Lines that span the x-axis
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, -scale.y, -scale.z),
		colliderCenter + glm::vec3(-scale.x, -scale.y, -scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, -scale.y, scale.z),
		colliderCenter + glm::vec3(-scale.x, -scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, scale.y, -scale.z),
		colliderCenter + glm::vec3(-scale.x, scale.y, -scale.z),
	colliderParent->componentParent, debugLinesColor);
	debugLines->AddLine(colliderCenter + glm::vec3(scale.x, scale.y, scale.z),
		colliderCenter + glm::vec3(-scale.x, scale.y, scale.z),
	colliderParent->componentParent, debugLinesColor);
}
#endif //DEBUG

void Collider::Update(float gameTime)
{
	glm::vec3 colliderCenter = colliderParent->componentParent->GetTranslation() + positionOffset;
	glm::vec3 colliderStart = colliderCenter - scale;
	glm::vec3 colliderEnd = colliderCenter + scale;

#ifdef DEBUG
	CreateDebugLines();
#endif
}

//Calculates and returns the projections of this rigidbody on the x, y, and z axes.
Collider::ColliderProjections Collider::CalculateProjections(bool addVelocity, bool addStoredVelocity, float gameTime)
{
	ColliderProjections projections = ColliderProjections();

	glm::vec3 colliderCenter = (colliderParent->componentParent->GetTranslation() + positionOffset);

	glm::vec3 velocity = colliderParent->GetVelocity();
	glm::vec3 storedVelocity = colliderParent->GetStoredVelocity();

	glm::mat4 colliderMatrix = colliderParent->componentParent->GetTransform() * glm::translate(glm::mat4(1), positionOffset) * glm::scale(glm::mat4(1), scale);

	glm::vec3 ihat = glm::normalize(glm::vec3(colliderMatrix[0][0], colliderMatrix[1][0], colliderMatrix[2][0]));
	glm::vec3 jhat = glm::normalize(glm::vec3(colliderMatrix[0][1], colliderMatrix[1][1], colliderMatrix[2][1]));
	glm::vec3 khat = glm::normalize(glm::vec3(colliderMatrix[0][2], colliderMatrix[1][2], colliderMatrix[2][2]));

	projections.x[0] = glm::dot(colliderCenter, glm::vec3(1,0,0)) + (velocity.x * addVelocity * gameTime) + (storedVelocity.x * addStoredVelocity * gameTime) - scale.x;
	projections.x[1] = glm::dot(colliderCenter, glm::vec3(1,0,0)) + (velocity.x * addVelocity * gameTime) + (storedVelocity.x * addStoredVelocity * gameTime) + scale.x;
	projections.y[0] = glm::dot(colliderCenter, glm::vec3(0,1,0)) + (velocity.y * addVelocity * gameTime) + (storedVelocity.y * addStoredVelocity * gameTime) - scale.y;
	projections.y[1] = glm::dot(colliderCenter, glm::vec3(0,1,0)) + (velocity.y * addVelocity * gameTime) + (storedVelocity.y * addStoredVelocity * gameTime) + scale.y;
	projections.z[0] = glm::dot(colliderCenter, glm::vec3(0,0,1)) + (velocity.z * addVelocity * gameTime) + (storedVelocity.z * addStoredVelocity * gameTime) - scale.z;
	projections.z[1] = glm::dot(colliderCenter, glm::vec3(0,0,1)) + (velocity.z * addVelocity * gameTime) + (storedVelocity.z * addStoredVelocity * gameTime) + scale.z;

	return projections;
}