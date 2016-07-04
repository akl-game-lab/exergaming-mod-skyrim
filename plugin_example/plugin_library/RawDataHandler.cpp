#include "RawDataHandler.h"

RawDataHandler::RawDataHandler()
{
	filePath = "Raw_Data.json";
	refresh();
}

void RawDataHandler::refresh()
{
	data = getJSON(filePath);
}

void RawDataHandler::clear()
{
	std::ofstream rawDataFile;
	rawDataFile.open(filePath);
	rawDataFile << "";
	rawDataFile.close();
}

int RawDataHandler::getWorkoutCount()
{
	debug.entry();
	int count = data["data"]["workouts"].size();
	debug.write(std::to_string(count));
	debug.exit();
	return count;
}

json RawDataHandler::getWorkout(int workoutNumber)
{
	return data["data"]["workouts"][workoutNumber];
}

std::string RawDataHandler::getResponseCode()
{
	std::string started = data["data"]["started"];
	if (!started.empty() && started == "true")
	{
		return "200";
	}
	std::string errorCode = data["data"]["errorCode"];
	if (!errorCode.empty())
	{
		return errorCode;
	}
	return "200";
}