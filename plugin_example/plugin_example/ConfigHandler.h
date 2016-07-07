#include "JSONHandler.h"

#ifndef __CONFIGHANDLER_H_INCLUDED__
#define __CONFIGHANDLER_H_INCLUDED__

#ifdef COMPILE_MYLIBRARY   
#define MYLIBRARY_EXPORT __declspec(dllexport) 
#else   
#define MYLIBRARY_EXPORT __declspec(dllimport) 
#endif

class MYLIBRARY_EXPORT ConfigHandler : public JSONHandler{
public:
	ConfigHandler();

	__int64 getConfigProperty(std::string propertyName);

	void setConfigProperty(std::string propertyName, __int64 value);

	void getDefaultData();
};
#endif