// test_suite.cpp : Defines the entry point for the console application.
//

#include "plugin_library\PluginLibrary.h"
#include "iostream"
#include "string"

PluginFunctions pluginFunctions;
RawDataHandler rawData;

int main()
{
	std::cout << "Testing getWorkoutsString()\n";
	std::cout << pluginFunctions.getWorkoutsString(12);
	getchar();
}

