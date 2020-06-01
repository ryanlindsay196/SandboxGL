#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Texture.h"
#include "ManagerClasses/TextureManager.h"
#include <gtc/type_ptr.hpp>
//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

void Shader::Initialize(TextureManager * in_textureManager, char* vertexPath, char* fragmentPath, char* materialPath)
{
	textureManager = in_textureManager;

	if (materialPath)
		LoadMaterial(materialPath);
	else
	{
		SetVertexShader(vertexPath);
		SetFragmentShader(fragmentPath);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

	}
	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//textures.clear();
	//for (char* texturePath : texturePaths)
	//{
	//	//if(TextureManager::textures.find(texturePath))
	//	
	//	textures.push_back(textureManager->LoadNewTexture(texturePath));
	//	//textures[textures.size() - 1]->SetDefaultTextureParameters();
	//	//textures[textures.size() - 1]->LoadTexture(texturePath);
	//}
}

void Shader::LoadMaterial(char * materialPath)
{
	std::ifstream materialFile(materialPath);

	std::string line;
	//std::vector<char*> texturePathsForShader;
	char* VertexPath = (char*)"";
	char* FragmentPath = (char*)"";
	char* currentShader = (char*)"";

	while (getline(materialFile, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		if (line.substr(0, 2) == "&&")
		{
			currentShader = ((char*)line.erase(0, 2).c_str());
			std::string tempTok[2];

			tempTok[0] = line;
			tempTok[0] = strtok((char*)tempTok[0].c_str(), " ");
			tempTok[1] = line;
			tempTok[1] = tempTok[1].substr(tempTok[1].find_first_of(" ") + 1);
			if (tempTok[0] == (char*)"VertexShader")
				SetVertexShader((char*)tempTok[1].c_str());
			else if (tempTok[0] == (char*)"FragmentShader")
				SetFragmentShader((char*)tempTok[1].c_str());
		}
		else if (line == "textures")
		{
			while (getline(materialFile, line) && line != "}")
			{
				//Remove tabs
				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				//Skip "{"
				if (line == "{")
					continue;
				else if (line == "}")
					break;
				//Remove spaces
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

				//Get the texture path
				std::string newTexturePath = line.substr(line.find_first_of(":") + 1);
				AddNewTexture((char*)newTexturePath.c_str());
				//texturePathsForShader.push_back(newTexturePath.copy(, 0, newTexturePath.size()).c_str();
				//texturePathsForShader.push_back((char*)(newTexturePath).c_str());
				//newTexturePath = line;
				//memcpy(texturePathsForShader[texturePathsForShader.size() - 1], (char*)newTexturePath.c_str(), strlen(newTexturePath.c_str()));

			}
		}
	}

	//if (VertexPath == (char*)"")
	//	VertexPath = (char*)"Resources/Shaders/VertexDefault.glsl";
	//if (FragmentPath == (char*)"")
	//	FragmentPath = (char*)"Resources/Shaders/FragmentDefault.glsl";

	materialFile.close();
}


void Shader::AddNewTexture(char * texturePath)
{
	textures.push_back(textureManager->LoadNewTexture(texturePath));
}

void Shader::ClearTextures()
{
	textures.clear();
}

void Shader::BindTextures()
{
	for (unsigned int i = 0; i < textures.size(); i++)
		textures[i]->UseTexture(i);

	SetShaderUniform_veci1((char*)"albedoMap", textures[0]->GetTextureID());
	SetShaderUniform_veci1((char*)"normalMap", textures[1]->GetTextureID());
		//glBindTexture(GL_TEXTURE_2D, (GLuint)textures[0]->GetTextureID());
}

//TODO: Create shader manager cpp and h files that function like the other manager files
//TODO: Load shader from material file. Materials will have paths to the vertex and fragment shaders, and have all default uniforms set.

void Shader::SetVertexShader(char* vertexPath)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	std::ifstream vShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		std::string vString = vShaderStream.str();
		vertexShaderSource = (char*)vString.c_str();
		std::cout << "Vertex shader successfully read" << std::endl;

		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::VERTEX_SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		else
			std::cout << "Shader compiled successfully" << std::endl;

		vShaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR:VERTEX_SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	//vertexShaderSource = "#version 330 core\n"
	//	"layout(location = 0) in vec3 aPos;\n"
	//	"layout (location = 1) in vec3 aColor;\n"
	//	"out vec3 ourColor;\n"
	//
	//	"void main()\n"
	//	"{\n"
	//	"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	//	"ourColor = aColor;\n"
	//	"}";
}

void Shader::SetFragmentShader(char* fragmentPath)
{

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::ifstream fShaderFile;
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fShaderFile.open(fragmentPath);
		std::stringstream fShaderStream;
		fShaderStream << fShaderFile.rdbuf();
		std::string fString = fShaderStream.str();
		fragmentShaderSource = (char*)fString.c_str();
		std::cout << "Fragment shader succesfully read" << std::endl;

		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		int success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::FRAGMENT_SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		else
			std::cout << "Shader compiled successfully" << std::endl;

		fShaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR:FRAGMENT_SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	//fragmentShaderSource = "#version 330 core\n"
	//	"out vec4 FragColor;\n"
	//	"in vec3 ourColor;\n"
	//	"void main()\n"
	//	"{\n"
	//	"FragColor = vec4(ourColor, 1.0);\n"
	//	"}\n";
}

std::vector<Texture*> Shader::GetTextures()
{
	return textures;
}

unsigned int Shader::GetShaderProgram()
{
	return shaderProgram;
}

void Shader::UseShader()
{
	glUseProgram(shaderProgram);
}

int Shader::GetShaderUniform_vec1(char * uniformName)
{
	return 0;
}

void Shader::SetShaderUniform_vec1(char * uniformName, int uniformValue)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1f(uniformLocation, (GLfloat)uniformValue);
}

void Shader::SetShaderUniform_vec2(char * uniformName, int x, int y)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2f(uniformLocation, (GLfloat)x, (GLfloat)y);
}

void Shader::SetShaderUniform_vec3(char * uniformName, int x, int y, int z)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3f(uniformLocation, (GLfloat)x, (GLfloat)y, (GLfloat)z);
}

void Shader::SetShaderUniform_vec3(char * uniformName, glm::vec3 & value)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3fv(uniformLocation, 1, &value[0]);
}

void Shader::SetShaderUniform_vec4(char * uniformName, int x, int y, int z, int w)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4f(uniformLocation, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}

void Shader::SetShaderUniform_veci1(char * uniformName, int uniformValue)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1i(uniformLocation, uniformValue);
}

void Shader::SetShaderUniform_veci2(char * uniformName, int x, int y)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2i(uniformLocation, x, y);
}

void Shader::SetShaderUniform_veci3(char * uniformName, int x, int y, int z)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3i(uniformLocation, x, y, z);
}

void Shader::SetShaderUniform_veci4(char * uniformName, int x, int y, int z, int w)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4i(uniformLocation, x, y, z, w);
}

void Shader::SetShaderUniform_mat4fv(char * uniformName, glm::mat4 matrix)
{
	unsigned int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}
