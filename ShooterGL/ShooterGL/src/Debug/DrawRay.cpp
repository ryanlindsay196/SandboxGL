#include "DrawRay.h"
#include "ManagerClasses/CameraManager.h"

DrawRay* DrawRay::instance = 0;

DrawRay::DrawRay()
{
}

DrawRay * DrawRay::GetInstance()
{
	if (instance == nullptr)
		instance = new DrawRay();
	return instance;
}

void DrawRay::DrawLine(glm::vec3 startPos, glm::vec3 endPos, const glm::mat4& parentTransform)
{
	Line l = Line(startPos, endPos);
	CameraManager* cameraManager = CameraManager::GetInstance();
	glm::mat4 mvp = cameraManager->GetCamera(0)->projectionMatrix * cameraManager->GetCamera(0)->cameraViewMatrix * parentTransform;
	l.setMVP(mvp);
	l.setColor(glm::vec3(0, 1, 0));
	l.draw();
}
