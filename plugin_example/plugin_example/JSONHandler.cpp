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
json JSONHandler::getJSON()
{
	json JSONData = {};
	if (fileExists(filePath))
	{
		std::ifstream inputFile(filePath);
		inputFile >> JSONData;
		inputFile.close();
	}
	return JSONData;
}

//Saves the given json object to the given file
void JSONHandler::saveJSON()
{
	std::ofstream outputFile(filePath);
	outputFile << data.dump(4).c_str();
	outputFile.close();
}