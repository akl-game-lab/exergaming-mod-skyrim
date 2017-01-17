#include "ConfigHandler.h"

ConfigHandler::ConfigHandler()
{
	filePath = "Config.json";
	if (!fileExists())
	{
		reset();
	}
	else
	{
		content = getJSON();
	}
}

__int64 ConfigHandler::getConfigProperty(std::string propertyName)
{
	return (__int64)content[propertyName];
}

void ConfigHandler::setConfigProperty(std::string propertyName, __int64 value)
{
	content[propertyName] = value;
}

void ConfigHandler::getDefaultData()
{
	content = {
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

void ConfigHandler::save()
{
	saveJSON();
}