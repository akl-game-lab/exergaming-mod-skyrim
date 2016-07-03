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

namespace plugin
{

	DebugHandler debug;
	/**********************************************************************************************************
	*	Globals
	*/
	std::string WEB_SERVICE_DIR = "Data\\webserviceTest\\Release";

	/**********************************************************************************************************
	*	Handlers
	*/

	ConfigHandler config;

	RawDataHandler rawData;

	WeekHandler weekHandler;

	/**********************************************************************************************************
	*	Plugin Helpers
	*/

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
		std::string weight = workout["weight"];
		std::string health = workout["health"];
		std::string stamina = workout["stamina"];
		std::string magicka = workout["magicka"];
		return weight + "," + health + "," + stamina + "," + magicka;
	}

	//Returns the week number that the workout date passed is in where week 1 is the week that the first workout was synced
	int getWeekForWorkout(long int firstTime, long int workoutTime) {
		return (((workoutTime - firstTime) / 604800) + 1);
	}

	//Returns the current date
	__int64 currentDate()
	{
		time_t t;
		time(&t);
		return t;
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

		__int64 startDate = _atoi64(config.getConfigProperty("startDate").c_str());
		__int64 lastWorkoutDate = _atoi64(config.getConfigProperty("lastWorkoutDate").c_str());
		__int64 firstWorkoutDate = _atoi64(config.getConfigProperty("firstWorkoutDate").c_str());

		int workoutCount = std::stoi(config.getConfigProperty("workoutCount").c_str());
		int weeksWorkedOut = std::stoi(config.getConfigProperty("weeksWorkedOut").c_str());
		int avgPointsPerWorkout = std::stoi(config.getConfigProperty("avgPointsPerWorkout").c_str());
		int totalPoints = std::stoi(config.getConfigProperty("totalPoints").c_str());
		int workoutsThisWeek = std::stoi(config.getConfigProperty("workoutsThisWeek").c_str());

		int workoutPoints = (int)workout["health"] + (int)workout["stamina"] + (int)workout["magicka"];

		if (firstWorkoutDate == 0 && workoutCount == 0 && (int)workout["workoutDate"] > startDate)
		{
			firstWorkoutDate = workout["workoutDate"];
			config.setConfigProperty("firstWorkoutDate", std::to_string(firstWorkoutDate));
		}

		int workoutsWeek = getWeekForWorkout(firstWorkoutDate, workout["workoutDate"]);
		int lastWorkoutsWeek = getWeekForWorkout(firstWorkoutDate, lastWorkoutDate);

		//if workout is for before the player synced their account
		if ((int)workout["workoutDate"] < startDate || (int)workout["workoutDate"] < firstWorkoutDate)
		{
			config.setConfigProperty("lastSyncDate", std::to_string(currentDate()));
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

		config.setConfigProperty("lastSyncDate", std::to_string(currentDate()));
		config.setConfigProperty("workoutCount", std::to_string(workoutCount));
		config.setConfigProperty("weeksWorkedOut", std::to_string(weeksWorkedOut));
		config.setConfigProperty("avgPointsPerWorkout", std::to_string(avgPointsPerWorkout));
		config.setConfigProperty("totalPoints", std::to_string(totalPoints));
		config.setConfigProperty("workoutsThisWeek", std::to_string(workoutsThisWeek));
		config.setConfigProperty("lastWorkoutDate", std::to_string(lastWorkoutDate));
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
		


		/*
		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		std::string cmdString = "\"" + exePath + "\" " + exeParams;
		LPCWSTR cmd = s2ws(cmdString);

		// start the program up
		CreateProcess(
			NULL,   // the path
			TEXT(cmd),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi           // Pointer to PROCESS_INFORMATION structure
			);
			// Close process and thread handles. 
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);*/

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
			if ((int)workout["weight"] > 0)
			{
				weekHandler.addWorkout(std::stoi(config.getConfigProperty("startDate")), workout);

				if (workoutNumber > 0)
				{
					workoutsAsString = workoutsAsString + ";";
				}

				workoutsAsString = workoutsAsString + workoutToString(workout);
			}
		}
		return workoutsAsString;
	}
}