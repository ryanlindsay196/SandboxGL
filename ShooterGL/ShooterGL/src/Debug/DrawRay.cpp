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

void DrawRay::DrawLine(glm::vec3 startPos, glm::vec3 endPos, glm::mat4 mvp)
{
	Line l = Line(startPos, endPos);
	CameraManager* cameraManager = CameraManager::GetInstance();
	mvp = cameraManager->GetCamera(0)->cameraViewMatrix * mvp;
	mvp = cameraManager->GetCamera(0)->projectionMatrix * mvp;
	l.setMVP(mvp);
	l.setColor(glm::vec3(1, 1, 0));
	l.draw();
}
