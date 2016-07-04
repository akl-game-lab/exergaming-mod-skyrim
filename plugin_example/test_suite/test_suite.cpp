// test_suite.cpp : Defines the entry point for the console application.
//

#include "plugin_library\PluginLibrary.h"
#include "iostream"
#include "string"

PluginFunctions pluginFunctions;

int main()
{
	std::cout << "Testing isOldSave()\n";
	if (pluginFunctions.isOldSave(pluginFunctions.currentDate()))
	{
		std::cout << "Failure";
	}
	else
	{
		std::cout << "Success";
	}
	getchar();
}

