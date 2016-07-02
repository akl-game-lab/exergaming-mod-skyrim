#include "fstream"
#include "json.hpp"
using json = nlohmann::json;

#ifndef __JSONHANDLER_H_INCLUDED__
#define __JSONHANDLER_H_INCLUDED__
class JSONHandler
{
protected:
	std::string filePath;
	json data;

public:
	//Checks if the given file exists
	bool fileExists(const std::string& name);

	//Gets a json object from the given file
	json getJSON(std::string fileName);

	//Saves the given json object to the given file
	void saveJSON(std::string fileName, json data);
};
#endif