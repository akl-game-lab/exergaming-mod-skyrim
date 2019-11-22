#include "RawDataHandler.h"

RawDataHandler::RawDataHandler()
{
	filePath = "Raw_Data.json";
	refresh();
}

void RawDataHandler::refresh()
{
	content = getJSON();
}

void RawDataHandler::getDefaultData()
{
	content = { "data",{} };
}

void RawDataHandler::clear()
{
	content = {};
	saveJSON();
}

int RawDataHandler::getWorkoutCount()
{
	int count = content["data"]["workouts"].size();
	debug.write("getting workout count\n\nReturning:" + std::to_string(count));
	return count;
}

json RawDataHandler::getWorkout(int workoutNumber)
{
	return content["data"]["workouts"][workoutNumber];
}

int RawDataHandler::getResponseCode()
{	
	//Handle and return the server response
	try {
		json responseCodeJson = content["responseCode"];
		if (!responseCodeJson.empty())
		{
			std::string responseCodeString = responseCodeJson;
			return std::stoi(responseCodeString);
		}
	}
	catch (...) {}

	//Default Server error
	return 503;
}

void RawDataHandler::setData(json newData)
{
	clear();
	content = newData;
	saveJSON();
}