// test_suite.cpp : Defines the entry point for the console application.
//

#include "plugin_example\PluginLibrary.h"
#include "iostream"
#include "string"

PluginFunctions pluginFunctions;
RawDataHandler rawData;

int main()
{
	if (pluginFunctions.isOldSave(1466208462))
	{
		std::cout << "Old save detected.";
	}
	std::cout << pluginFunctions.startNormalFetch("Skyrim","paul@paulralph.name");
	Sleep(5000);
	std::cout << pluginFunctions.getWorkoutsString(12) + "\n";
	getchar();
}