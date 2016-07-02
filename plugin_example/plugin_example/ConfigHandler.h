#include "JSONHandler.h"

class ConfigHandler : public JSONHandler{
public:
	ConfigHandler();

	std::string getConfigProperty(std::string propertyName);

	void setConfigProperty(std::string propertyName, std::string value);
};