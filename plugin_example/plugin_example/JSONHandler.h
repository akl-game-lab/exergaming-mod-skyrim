#include "fstream"
#include "json.hpp"
#include "DebugHandler.h"

using json = nlohmann::json;

#ifndef __JSONHANDLER_H_INCLUDED__
#define __JSONHANDLER_H_INCLUDED__
class JSONHandler
{
protected:
	std::string filePath;
	json data;
	DebugHandler debug;

	//Checks if the given file exists
	bool fileExists(const std::string& name);
	
	//Gets a json object from the given file
	json getJSON();

	//Saves the given json object to the given file
	void saveJSON();
};
#endif