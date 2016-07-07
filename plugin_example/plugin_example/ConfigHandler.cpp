#include "ConfigHandler.h"

ConfigHandler::ConfigHandler()
{
	filePath = "Config.json";
	if (!fileExists(filePath))
	{
		data = {
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
		saveJSON();
	}
	else
	{
		data = getJSON();
	}
}

__int64 ConfigHandler::getConfigProperty(std::string propertyName)
{
	return (__int64)data[propertyName];
}

void ConfigHandler::setConfigProperty(std::string propertyName, __int64 value)
{
	data[propertyName] = value;
	saveJSON();
}