// test_suite.cpp : Defines the entry point for the console application.
//

#include "plugin_library\PluginLibrary.h"
#include "iostream"
#include "string"

PluginFunctions pluginFunctions;
RawDataHandler rawData;

int main()
{
	std::cout << "Testing isOldSave()\n";
	if (pluginFunctions.isOldSave(pluginFunctions.currentDate()))
	{
		std::cout << "Failure\n";
	}
	else
	{
		std::cout << "Success\n";
	}
	std::cout << "Testing startNormalFetch()\n";
	pluginFunctions.startNormalFetch("Skyrim","username");
	std::cout << "Success\n";

	std::cout << "Testing getWorkoutsString()\n|";
	std::cout << pluginFunctions.getWorkoutsString(12);
	std::cout << "|";

	getchar();
}

