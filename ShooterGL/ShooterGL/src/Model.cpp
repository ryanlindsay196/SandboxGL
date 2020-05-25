#include "Model.h"
#include "Shader.h"
#include <glad/glad.h>
#include "TextureManager.h"

Model::Model()
{

}

Model::~Model()
{
}

void Model::Initialize(TextureManager* textureManager)
{
	m_textureManager = textureManager;

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0 * 1, 2 * 1, 3 * 1,   // first triangle		tr/bl/tl
		0 * 1, 2 * 1, 1 * 1    // second triangle
	};


	glGenBuffers(1, &VBO);//

	glGenVertexArrays(1, &VAO);//
	//Initialization code (done once (unless your object frequently changes))
	//a. bind vertex array object
	glBindVertexArray(VAO);//
	//2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	SetDefaultShaders();
}

void Model::SetDefaultShaders()
{
	if (shader != nullptr)
		delete(shader);
	shader = new Shader();
	shader->Initialize(m_textureManager, (char*)"Shaders/VertexDefault.glsl", (char*)"Shaders/FragmentDefault.glsl", { (char*)"Resources/Textures/Rick.jpg", (char*)"Resources/Textures/Sword_low_Blade_Normal.png" });
}

void Model::Update()
{
}

void Model::Render()
{
	//Drawing code (in render loop)
	glUseProgram(shader->GetShaderProgram());
	glBindVertexArray(VAO);
	shader->BindTextures();
	//TODO: Set the 6 to the amount of indices
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}
