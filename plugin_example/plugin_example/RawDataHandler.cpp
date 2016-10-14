#include "RawDataHandler.h"

RawDataHandler::RawDataHandler()
{
	filePath = "Raw_Data.json";
	refresh();
}

void RawDataHandler::refresh()
{
	data = getJSON();
}

void RawDataHandler::getDefaultData()
{
	data = { "data",{} };
}

void RawDataHandler::clear()
{
	data = {};
	saveJSON();
}

int RawDataHandler::getWorkoutCount()
{
	std::string response = data;
	if (response.compare("null") == 0) 
	{
		return -1;
	}
	int count = data["data"]["workouts"].size();
	return count;
}

json RawDataHandler::getWorkout(int workoutNumber)
{
	return data["data"]["workouts"][workoutNumber];
}

int RawDataHandler::getResponseCode(std::string type)
{	
	try {
		json errorCode = data["data"]["errorCode"];
		if (!errorCode.empty())
		{
			std::string errorCodeString = data["data"]["errorCode"];
			return 404;
		}
	}
	catch (...) {}

	try {
		std::string started = data["data"]["started"];

		if (started.compare("true") == 0)
		{
			return 200;
		}
	}
	catch(...){}
	return 500;
}

void RawDataHandler::setData(json newData)
{
	clear();
	data = newData;
	saveJSON();
}