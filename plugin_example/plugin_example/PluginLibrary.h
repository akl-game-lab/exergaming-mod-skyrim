#include "string"
#include "Windows.h"
#include "ShellApi.h"
#include "ctime"
#include "tchar.h"
#include "vector"
#include "json.hpp"
#include "ConfigHandler.h"
#include "DebugHandler.h"
#include "RawDataHandler.h"
#include "WeekHandler.h"

using json = nlohmann::json;

/**********************************************************************************************************
*	Globals
*/

int WEIGHT = 0;
int HEALTH = 1;
int STAMINA = 2;
int MAGICKA = 3;
char ITEM_SEPARATOR = ';';
char FIELD_SEPARATOR = ',';
std::string WEB_SERVICE_DIR = "Data\\webserviceTest\\Release";

/**********************************************************************************************************
*	Handlers
*/

DebugHandler debug;
ConfigHandler config;
RawDataHandler rawData;
WeekHandler weekHandler;

/**********************************************************************************************************
*	Helpers
*/

//Returns the current date
__int64 currentDate()
{
	time_t t;
	time(&t);
	return t;
}

//Splits a string by the given delimeter
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

//Converts a json representation of a workout to a formatted string
std::string workoutToString(json workout)
{
	std::string weight = std::to_string((int)workout["weight"]);
	std::string health = std::to_string((int)workout["health"]);
	std::string stamina = std::to_string((int)workout["stamina"]);
	std::string magicka = std::to_string((int)workout["magicka"]);
	return weight + "," + health + "," + stamina + "," + magicka;
}

//Returns the week number that the workout date passed is in where week 1 is the week that the first workout was synced
int getWeekForWorkout(__int64 firstTime, __int64 workoutTime) {
	return (((workoutTime - firstTime) / SECONDS_PER_WEEK) + 1);
}

//Returns a float representation of the number of levels gained from the workout passed to the method
float configure(json workout, int level)
{
	// the amount of extra experience (points) needed per specified number of levels needed 
	float expIncreaseRate = 0.1;
	// every x levels there should be improvement
	int levelImprovement = 12;
	int estimatedLevelsPerWeek = 3;
	float levelsGained = 0;

	__int64 startDate = config.getConfigProperty("startDate");
	__int64 lastWorkoutDate = config.getConfigProperty("lastWorkoutDate");
	__int64 firstWorkoutDate = config.getConfigProperty("firstWorkoutDate");

	int workoutCount = config.getConfigProperty("workoutCount");
	int weeksWorkedOut = config.getConfigProperty("weeksWorkedOut");
	int avgPointsPerWorkout = config.getConfigProperty("avgPointsPerWorkout");
	int totalPoints = config.getConfigProperty("totalPoints");
	int workoutsThisWeek = config.getConfigProperty("workoutsThisWeek");

	int workoutPoints = (int)workout["health"] + (int)workout["stamina"] + (int)workout["magicka"];

	if (firstWorkoutDate == 0 && workoutCount == 0 && (__int64)workout["workoutDate"] > startDate)
	{
		firstWorkoutDate = workout["workoutDate"];
		config.setConfigProperty("firstWorkoutDate", firstWorkoutDate);
	}

	int workoutsWeek = getWeekForWorkout(firstWorkoutDate, workout["workoutDate"]);
	int lastWorkoutsWeek = getWeekForWorkout(firstWorkoutDate, lastWorkoutDate);

	//if workout is for before the player synced their account
	if ((int)workout["workoutDate"] < startDate || (int)workout["workoutDate"] < firstWorkoutDate)
	{
		config.setConfigProperty("lastSyncDate", currentDate());
		return 0;
	}
	else
	{
		totalPoints = totalPoints + workoutPoints;
		workoutCount = workoutCount + 1;

		if (workoutsWeek == 1)
		{
			if (weeksWorkedOut == 0)
			{
				weeksWorkedOut = 1;
			}
			/*When the workout is part of the first week*/
			lastWorkoutDate = workout["workoutDate"];
			workoutsThisWeek = workoutsThisWeek + 1;
			levelsGained = 1.0;
		}
		else if (workoutsWeek == lastWorkoutsWeek)
		{
			/*When the workout is part of the current week*/
			lastWorkoutDate = workout["workoutDate"];
			workoutsThisWeek = workoutsThisWeek + 1;
		}
		else
		{
			avgPointsPerWorkout = (totalPoints / workoutCount);

			if (workoutsWeek > lastWorkoutsWeek)
			{
				/*When the workout is in a new week*/
				lastWorkoutDate = workout["workoutDate"];
				weeksWorkedOut = weeksWorkedOut + 1;
				workoutsThisWeek = 1;
			}
		}

		if (levelsGained == 0)
		{
			float avgWorkoutsPerWeek = ((float)(workoutCount - workoutsThisWeek) / (weeksWorkedOut - 1));
			levelsGained = ((estimatedLevelsPerWeek * workoutPoints) / (avgPointsPerWorkout * avgWorkoutsPerWeek));
			levelsGained = levelsGained / (1 + ((level / levelImprovement) * expIncreaseRate));
		}
	}

	config.setConfigProperty("lastSyncDate", currentDate());
	config.setConfigProperty("workoutCount", workoutCount);
	config.setConfigProperty("weeksWorkedOut", weeksWorkedOut);
	config.setConfigProperty("avgPointsPerWorkout", avgPointsPerWorkout);
	config.setConfigProperty("totalPoints", totalPoints);
	config.setConfigProperty("workoutsThisWeek", workoutsThisWeek);
	config.setConfigProperty("lastWorkoutDate", lastWorkoutDate);
	return levelsGained;
}

//Makes the service call to get raw data from the server
void makeServiceCall(std::string type, std::string username, std::string fromDate, std::string toDate)
{
	rawData.clear();

	std::string exeParams = type + " " + username + " " + fromDate + " " + toDate;
	LPCSTR swExeParams = exeParams.c_str();

	//Set the executable path
	std::string exePath = WEB_SERVICE_DIR + "\\webserviceTest.exe";
	LPCSTR swExePath = exePath.c_str();

	//Execute the code that fetches the xml and stores it in the skyrim folder.
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = (LPCTSTR)swExePath;
	ShExecInfo.lpParameters = (LPCTSTR)swExeParams;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWMINNOACTIVE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	rawData.refresh();
}

//Updates the Weeks.xml file to contain all workouts logged to date
std::string updateWeeks(int level)
{
	rawData.refresh();
	std::string workoutsAsString = "";
	int health = 0, stamina = 0, magicka = 0;
	int workoutCount = rawData.getWorkoutCount();

	//for each workout
	for (int workoutNumber = 0; workoutNumber < workoutCount; workoutNumber++)
	{
		json workout = rawData.getWorkout(workoutNumber);
		//adjust the config to account for the new workout and gets the workouts weight
		workout["weight"] = configure(workout, level);
		if ((float)workout["weight"] > 0)
		{
			weekHandler.addWorkout(workout);

			if (workoutNumber > 0)
			{
				workoutsAsString = workoutsAsString + ";";
			}

			workoutsAsString = workoutsAsString + workoutToString(workout);
		}
	}

	return workoutsAsString;
}