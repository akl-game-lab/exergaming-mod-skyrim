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

	__int64 getConfigProperty(std::string propertyName);

	void setConfigProperty(std::string propertyName, int value);
};
#endif