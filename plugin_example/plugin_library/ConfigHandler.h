#include "JSONHandler.h"

#ifdef COMPILE_MYLIBRARY   
#define MYLIBRARY_EXPORT __declspec(dllexport) 
#else   
#define MYLIBRARY_EXPORT __declspec(dllimport) 
#endif

#ifndef __CONFIGHANDLER_H_INCLUDED__
#define __CONFIGHANDLER_H_INCLUDED__

class MYLIBRARY_EXPORT ConfigHandler : public JSONHandler{
public:
	ConfigHandler();

	std::string getConfigProperty(std::string propertyName);

	void setConfigProperty(std::string propertyName, std::string value);
};
#endif