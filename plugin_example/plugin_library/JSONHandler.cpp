#include "JSONHandler.h"

//Checks if the given file exists
bool JSONHandler::fileExists(const std::string& name)
{
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

//Gets a json object from the given file
json JSONHandler::getJSON(std::string fileName)
{
	json JSONData = {};
	if (fileExists(fileName))
	{
		std::ifstream inputFile(fileName);
		inputFile >> JSONData;
		inputFile.close();
	}
	return JSONData;
}

//Saves the given json object to the given file
void JSONHandler::saveJSON(std::string fileName, json data)
{
	std::ofstream outputFile(fileName);
	outputFile << data.dump(4).c_str();
	outputFile.close();
}