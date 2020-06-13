#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Texture.h"
#include "ManagerClasses/TextureManager.h"
#include "ManagerClasses/ObjectManager.h"
#include "ManagerClasses/LightManager.h"
#include <gtc/type_ptr.hpp>
//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

void Shader::Initialize(ObjectManager * in_objectManager, char* vertexPath, char* fragmentPath, char* materialPath)
{
	textureManager = in_objectManager->textureManager;
	m_objectManager = in_objectManager;
	shaderProgram = glCreateProgram();

	if (materialPath)
		LoadMaterial(materialPath);
	else
	{
		SetVertexShader(vertexPath);
		SetFragmentShader(fragmentPath);
	}

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

	}
	glUseProgram(shaderProgram);

	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTES, &count);
	printf("Active Attributes: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveAttrib(shaderProgram, (GLuint)i, bufSize, &length, &size, &type, name);

		printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
	}

	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(shaderProgram, (GLuint)i, bufSize, &length, &size, &type, name);

		printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::LoadMaterial(char * materialPath)
{
	std::ifstream materialFile(materialPath);

	std::string line;
	//std::vector<char*> texturePathsForShader;
	//char* VertexPath = (char*)"";
	//char* FragmentPath = (char*)"";
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
		else if (line == "<ATTACH AND LINK SHADER>")
		{
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);
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
				std::string newTextureUniform = strtok((char*)line.c_str(), ":");
				AddNewTexture((char*)newTexturePath.c_str(), (char*)newTextureUniform.c_str());
			}
		}
		else if (line == "vec3")
		{
			while (getline(materialFile, line) && line != "}")
			{

				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				//Skip "{"
				if (line == "{")
					continue;
				else if (line == "}")
					break;
				std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line);

				glm::vec3 newVector = ParseMaterialVector(keyValuePair.second);
				SetShaderUniform_vec3((char*)keyValuePair.first.c_str(), newVector);
			}
		}
		else if (line == "float")
		{
			while (getline(materialFile, line) && line != "}")
			{

				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				//Skip "{"
				if (line == "{")
					continue;
				else if (line == "}")
					break;
				std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line);

				SetShaderUniform_vec1((char*)keyValuePair.first.c_str(), strtof(keyValuePair.second.c_str(), nullptr));
			}
		}
	}

	//SetShaderUniform_vec1("mat.ambient");

	//if (VertexPath == (char*)"")
	//	VertexPath = (char*)"Resources/Shaders/VertexDefault.glsl";
	//if (FragmentPath == (char*)"")
	//	FragmentPath = (char*)"Resources/Shaders/FragmentDefault.glsl";

	materialFile.close();
}

std::pair<std::string, std::string> Shader::GenerateKeyValuePair(std::string line)
{
	std::pair<std::string, std::string> newKeyValuePair;
	newKeyValuePair.second = line.substr(line.find_first_of(":") + 1);
	newKeyValuePair.first = strtok((char*)line.c_str(), ":");
	return newKeyValuePair;
}

glm::vec3 Shader::ParseMaterialVector(std::string line)
{
	if(line.find(",") == line.npos)
		return glm::vec3(strtof((char*)line.c_str(), nullptr));

	float x, y, z;
	std::string xLine, yLine, zLine;
	xLine = yLine = zLine = line;

	xLine = strtok((char*)xLine.c_str(), ",");
	
	yLine = yLine.substr(yLine.find_first_of(",") + 1);
	yLine = strtok((char*)yLine.c_str(), ",");

	zLine = zLine.substr(zLine.find_last_of(",") + 1);
	//zLine = strtok((char*)zLine.c_str(), ",");

	x = strtof((char*)xLine.c_str(), nullptr);
	y = strtof((char*)yLine.c_str(), nullptr);
	z = strtof((char*)zLine.c_str(), nullptr);
	return glm::vec3(x, y, z);
}


void Shader::AddNewTexture(char * texturePath, char* textureUniform)
{
	textures.push_back(textureManager->LoadNewTexture(texturePath));
	textureUniforms.push_back(textureUniform);
}

void Shader::ClearTextures()
{
	textures.clear();
}

void Shader::BindTextures()
{
	for (unsigned int i = 0; i < textures.size(); i++)
		textures[i]->UseTexture(i);

	//TODO: Get uniform names from material, make hash map using these names and the texture ID
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		SetShaderUniform_veci1((char*)textureUniforms[i].c_str(), textures[i]->GetTextureID());
	}
	
	//SetShaderUniform_veci1((char*)"albedoMap", textures[0]->GetTextureID());
	//SetShaderUniform_veci1((char*)"normalMap", textures[1]->GetTextureID());
	
	//glBindTexture(GL_TEXTURE_2D, (GLuint)textures[0]->GetTextureID());
}

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


		std::string line;
		while (getline(fShaderStream, line))
		{
			if (line == "#define NR_POINT_LIGHTS X")
			{
				line = "#define NR_POINT_LIGHTS " + std::to_string(m_objectManager->lightManager->TotalLights());
			}
			std::cout << line;
		}

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

void Shader::SetShaderUniform_vec1(char * uniformName, float uniformValue)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1f(uniformLocation, (GLfloat)uniformValue);
}

void Shader::SetShaderUniform_vec2(char * uniformName, float x, float y)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2f(uniformLocation, (GLfloat)x, (GLfloat)y);
}

void Shader::SetShaderUniform_vec3(char * uniformName, float x, float y, float z)
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

void Shader::SetShaderUniform_vec4(char * uniformName, float x, float y, float z, float w)
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
	glUseProgram(shaderProgram);
	unsigned int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}
