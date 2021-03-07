#include "GenerateSpreadsheet.h"
#include <iostream>
#include <fstream>

void GenerateSpreadsheet::Initialize(std::string fileName)
{
	file = new std::ofstream();
	file->open(fileName);
}

void GenerateSpreadsheet::AppendText(std::string text)
{
	(*file) << text;
}

void GenerateSpreadsheet::CloseFile()
{
	file->close();
}
