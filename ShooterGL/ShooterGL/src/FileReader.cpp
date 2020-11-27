#include "FileReader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"

//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

std::pair<std::string, std::string> GenerateKeyValuePair(std::string line, std::string delimiter)
{
	std::pair<std::string, std::string> newKeyValuePair;
	newKeyValuePair.second = line.substr(line.find_first_of(delimiter) + 1);
	newKeyValuePair.first = strtok((char*)line.c_str(), delimiter.c_str());
	return newKeyValuePair;
}

glm::vec3 ParseVector(std::string line)
{
	//If the vector is just one number instead of three
	if (line.find(",") == line.npos)
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

glm::quat ParseQuaternion(std::string line)
{
	//if (line.find(",") == line.npos)
	//	return glm::quat(strtof((char*)line.c_str(), nullptr));

	float x, y, z, w;
	std::string xLine, yLine, zLine, wLine;
	xLine = yLine = zLine = wLine = line;

	xLine = strtok((char*)xLine.c_str(), ",");

	yLine = yLine.substr(yLine.find_first_of(",") + 1);
	yLine = strtok((char*)yLine.c_str(), ",");

	zLine = zLine.substr(zLine.find_first_of(",") + 1);
	zLine = zLine.substr(zLine.find_first_of(",") + 1);
	zLine = strtok((char*)zLine.c_str(), ",");

	wLine = wLine.substr(wLine.find_last_of(",") + 1);

	x = strtof((char*)xLine.c_str(), nullptr);
	y = strtof((char*)yLine.c_str(), nullptr);
	z = strtof((char*)zLine.c_str(), nullptr);
	w = strtof((char*)wLine.c_str(), nullptr);
	return glm::quat(x, y, z, w);
}
