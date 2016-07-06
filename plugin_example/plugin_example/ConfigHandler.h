#include "JSONHandler.h"

#ifndef __CONFIGHANDLER_H_INCLUDED__
#define __CONFIGHANDLER_H_INCLUDED__

class ConfigHandler : public JSONHandler{
public:
	ConfigHandler();

	__int64 getConfigProperty(std::string propertyName);

	void setConfigProperty(std::string propertyName, int value);
};
#endif