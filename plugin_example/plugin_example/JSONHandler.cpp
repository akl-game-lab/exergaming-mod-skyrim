#include "JSONHandler.h"

//Checks if the given file exists
bool JSONHandler::fileExists()
{
	if (FILE *file = fopen(filePath.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

//Gets a json object from the given file
json JSONHandler::getJSON()
{
	json JSONData = {};
	if (fileExists() && !fileEmpty())
	{
		std::ifstream inputFile(filePath);
		inputFile >> JSONData;
		inputFile.close();
	}
	return JSONData;
}

void JSONHandler::reset()
{
	getDefaultData();
}

//Saves the given json object to the given file
void JSONHandler::saveJSON()
{
	std::ofstream outputFile(filePath);
	outputFile << data.dump(4).c_str();
	outputFile.close();
}

bool JSONHandler::fileEmpty()
{
	if (fileExists())
	{
		std::ifstream inputFile(filePath);
		return inputFile.peek() == std::ifstream::traits_type::eof();
	}
	return true;
}