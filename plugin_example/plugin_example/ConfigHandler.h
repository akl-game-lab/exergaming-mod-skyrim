#include "JSONHandler.h"

#ifndef __CONFIGHANDLER_H_INCLUDED__
#define __CONFIGHANDLER_H_INCLUDED__
class ConfigHandler : public JSONHandler{
public:
	ConfigHandler();

	std::string getConfigProperty(std::string propertyName);

	void setConfigProperty(std::string propertyName, std::string value);
};
#endif