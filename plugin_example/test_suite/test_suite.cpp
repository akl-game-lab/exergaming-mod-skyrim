// test_suite.cpp : Defines the entry point for the console application.
//

#include "plugin_library\PluginLibrary.h"
#include "iostream"
#include "string"

PluginFunctions pluginFunctions;
RawDataHandler rawData;

int main()
{
	//WEB_SERVICE_DIR = "C:\\Program\ Files\ (x86)\\Steam\\steamapps\\common\\Skyrim\\Data\\SKSE\\Plugins";
	
	std::cout << "Testing isOldSave()\n";
	if (pluginFunctions.isOldSave(pluginFunctions.currentDate()))
	{
		std::cout << "Failure\n";
	}
	else
	{
		std::cout << "Success\n";
	}
	
	std::cout << "Attempting to construct raw data.\n";

	json newRawData = {
		{ "startDate", 0 },
		{ "lastWorkoutDate", 0 },
		{ "lastSyncDate", 0 },
		{ "firstWorkoutDate", 0 },
		{ "workoutCount", 0 },
		{ "weeksWorkedOut", 0 },
		{ "avgPointsPerWorkout", 0 },
		{ "totalPoints", 0 },
		{ "workoutsThisWeek", 0 }
	};

	std::cout << "Attempting to set raw data.\n";

	rawData.setData(newRawData);

	std::cout << "Set raw data.\n";

	/*
	std::cout << rawData.getWorkoutCount();
	std::cout << "\n";
	std::cout << "Testing getWorkoutsString()\n|";
	std::cout << pluginFunctions.getWorkoutsString(12);
	std::cout << "|";*/
	getchar();
}

