#include "AnimationDataStructures.h"

glm::mat4 AiMat4ToGlmMat4(aiMatrix4x4 aiMat4)
{
		return glm::mat4(
			aiMat4.a1, aiMat4.b1, aiMat4.c1, aiMat4.d1,
			aiMat4.a2, aiMat4.b2, aiMat4.c2, aiMat4.d2,
			aiMat4.a3, aiMat4.b3, aiMat4.c3, aiMat4.d3,
			aiMat4.a4, aiMat4.b4, aiMat4.c4, aiMat4.d4
		);
}
