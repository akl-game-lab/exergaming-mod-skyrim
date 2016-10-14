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
	int count = data["data"]["workouts"].size();
	return count;
}

json RawDataHandler::getWorkout(int workoutNumber)
{
	return data["data"]["workouts"][workoutNumber];
}

int RawDataHandler::getResponseCode()
{	
	//Handle and return the server response
	try {
		json responseCode = data["responseCode"];
		if (!responseCode.empty())
		{
			return std::stoi(std::string(responseCode));
		}
	}
	catch (...) {}

	//Default Server error
	return 500;
}

void RawDataHandler::setData(json newData)
{
	clear();
	data = newData;
	saveJSON();
}