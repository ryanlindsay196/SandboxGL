#pragma once
#include <string>
class GenerateSpreadsheet {
	std::ofstream* file;
public:
	void Initialize(std::string fileName);
	void AppendText(std::string text);
	void CloseFile();
};