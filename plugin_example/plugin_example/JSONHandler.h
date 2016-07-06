#include "fstream"
#include "json.hpp"
#include "DebugHandler.h"

using json = nlohmann::json;

#ifndef __JSONHANDLER_H_INCLUDED__
#define __JSONHANDLER_H_INCLUDED__

#ifdef COMPILE_MYLIBRARY   
#define MYLIBRARY_EXPORT __declspec(dllexport) 
#else   
#define MYLIBRARY_EXPORT __declspec(dllimport) 
#endif

class MYLIBRARY_EXPORT JSONHandler
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