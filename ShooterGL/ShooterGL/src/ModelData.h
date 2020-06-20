#pragma once
#include <vector>
#include "Renderables/Mesh.h"

class ModelData
{
	unsigned int VAO, EBO, VBO;
	std::vector<Mesh> m_meshes;
};