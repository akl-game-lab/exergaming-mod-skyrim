// test_suite.cpp : Defines the entry point for the console application.
//

#include "plugin_example\PluginLibrary.h"
#include "iostream"
#include "string"

PluginFunctions pluginFunctions;
RawDataHandler rawData;

int main()
{
	std::cout << "Testing getWorkoutsFromBestWeek()\n";
	std::cout << pluginFunctions.getWorkoutsFromBestWeek(1462211085) + "\n";
	getchar();
}