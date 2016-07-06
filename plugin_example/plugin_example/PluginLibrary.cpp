#include "PluginLibrary.h"

/**********************************************************************************************************
*	Helpers
*/
	
//Splits a string by the given delimeter
std::vector<std::string> PluginFunctions::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

//Converts a json representation of a workout to a formatted string
std::string PluginFunctions::workoutToString(json workout)
{
	std::string weight = std::to_string((float)workout["weight"]);
	std::string health = std::to_string((int)workout["health"]);
	std::string stamina = std::to_string((int)workout["stamina"]);
	std::string magicka = std::to_string((int)workout["magicka"]);
	return weight + "," + health + "," + stamina + "," + magicka;
}

//Returns the week number that the workout date passed is in where week 1 is the week that the first workout was synced
int PluginFunctions::getWeekForWorkout(__int64 firstTime, __int64 workoutTime) {
	return (((workoutTime - firstTime) / SECONDS_PER_WEEK) + 1);
}

//Returns the day of the week that is the number of days from the start of the week, with the day of the configs startDate as the first day of the week.
int PluginFunctions::getDayOfConfigWeek(__int64 date)
{
	int dayOfWeek = weekHandler.getDayOfWeek(date) - weekHandler.getDayOfWeek(config.getConfigProperty("startDate"));
	if (dayOfWeek < 0)
	{
		return 7 + dayOfWeek;
	}
	return dayOfWeek;
}

//Returns a float representation of the number of levels gained from the workout passed to the method
float PluginFunctions::configure(json workout, int level)
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
void PluginFunctions::makeServiceCall(std::string type, std::string username, std::string fromDate, std::string toDate)
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
std::string PluginFunctions::updateWeeks(int level)
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
	
/**********************************************************************************************************
*	Functions
*/

//Returns the current date
__int64 PluginFunctions::currentDate()
{
	time_t t;
	time(&t);
	return t;
}

//Checks if the current save is old
bool PluginFunctions::isOldSave(int creationDate)
{
	int lastSyncDate = config.getConfigProperty("lastSyncDate");
	return creationDate < lastSyncDate;
}

//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
std::string PluginFunctions::getWorkoutsFromBestWeek(__int64 creationDate)
{
	int weekCount = weekHandler.getWeekCount();
	float bestWeeksWeight = 0;
	std::string bestWeeksWorkouts = "0,0,0,0";

	if (weekCount != 0) {
		for (int weekNumber = 0; weekNumber < weekCount; weekNumber++)
		{
			__int64 weekStartDate = weekHandler.getWeekStart(weekNumber);

			//if the week in frame is after the creation date or is the week the save was made
			if (weekStartDate > creationDate ||
				(weekStartDate < creationDate && creationDate - weekStartDate < SECONDS_PER_WEEK))
			{
				float thisWeeksWeight = 0;
				std::string thisWeeksWorkouts = "";
				json workouts = weekHandler.getWorkoutsFromWeek(weekNumber);

				//for each workout
				int useableWorkoutCount = 0;
				for (int workoutNumber = 0; workoutNumber < workouts.size(); workoutNumber++)
				{
					json workout = workouts[workoutNumber];
					__int64 workoutDate = (__int64)workout["workoutDate"];
					
					/*
					*	If it was done on a day of the week after that of the creation date add the weight to the total weight for this week
					*	As weeks don't start on Sunday in this implimentation, this is done via the difference between the day of week of the start date.
					*/

					int workoutDay = getDayOfConfigWeek(workoutDate);
					int creationDay = getDayOfConfigWeek(creationDate);

					if (workoutDate >= creationDate && workoutDay >= creationDay) {
						thisWeeksWeight += (float)workout["weight"];
						if (useableWorkoutCount > 0)
						{
							thisWeeksWorkouts = thisWeeksWorkouts + ";";
						}
						thisWeeksWorkouts = thisWeeksWorkouts + workoutToString(workout);
						useableWorkoutCount++;
					}
				}

				//if this week is better update the best week
				if (thisWeeksWeight > bestWeeksWeight)
				{
					bestWeeksWeight = thisWeeksWeight;
					bestWeeksWorkouts = thisWeeksWorkouts;
				}
			}
		}
	}

	//return the selected best weeks workouts as a string from the day of the week the save was made (format is W,H,S,M;W,H,S,M...).
	return bestWeeksWorkouts;
}

//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
std::string PluginFunctions::getLevelUpsAsString(std::string outstandingLevel, std::string workoutsString)
{
	std::string levelUps = "";

	//initialise the level ditribution with the outstanding points
	std::vector<std::string> outstandingLevelFields = split(outstandingLevel, FIELD_SEPARATOR);

	/*TO-DO
	Improve health, stamina, magicka reptition with a loop from 0-2;
	*/

	float totalHealth = 0;
	float totalStamina = 0;
	float totalMagicka = 0;
	float totalWeight = 0;

	if (outstandingLevelFields.size() == 3)
	{
		try
		{
			totalHealth = std::stof(outstandingLevelFields.at(HEALTH).c_str());
			totalStamina = std::stof(outstandingLevelFields.at(STAMINA).c_str());
			totalMagicka = std::stof(outstandingLevelFields.at(MAGICKA).c_str());
			totalWeight = (totalHealth + totalStamina + totalMagicka) / 10;
		}
		catch (const std::out_of_range& oor)
		{
		}
	}

	std::vector<std::string> workouts = split(workoutsString, ITEM_SEPARATOR);
	//loop through the given weeks workouts to see if a level up can be awarded
	for (int i = 0; i < workouts.size(); i++)
	{
		std::string workout = workouts.at(i);

		std::vector<std::string> workoutFields = split(workout, FIELD_SEPARATOR);

		try
		{
			int weight = std::stoi(workoutFields.at(WEIGHT));
			int health = std::stoi(workoutFields.at(HEALTH));
			int stamina = std::stoi(workoutFields.at(STAMINA));
			int magicka = std::stoi(workoutFields.at(MAGICKA));
			int total = health + stamina + magicka;

			if (total == 0)
			{
				break;
			}

			//get how much weight is needed to level up
			int weightNeeded = 1 - totalWeight;

			//use the minimum of the current workouts weight, and the weight needed to scale the current workouts points
			int scalingWeight = min(weightNeeded, weight);

			float healthUsed = ((health * 10) * scalingWeight) / total;
			float staminaUsed = ((stamina * 10) * scalingWeight) / total;
			float magickaUsed = (10 * scalingWeight) - (healthUsed + staminaUsed);

			totalHealth += healthUsed;
			totalStamina += staminaUsed;
			totalMagicka += magickaUsed;

			//if the weight of the current workout is greter than the weight needed then the player has levelled up
			if (weightNeeded <= weight)
			{
				//convert point distribution to ints
				int healthInt = round(totalHealth);
				int staminaInt = round(totalStamina);
				int magikaInt = 10 - (healthInt + staminaInt);

				//format the level up string
				std::string newLevelUp = std::to_string(healthInt) + "," + std::to_string(staminaInt) + "," + std::to_string(magikaInt);

				//add the new level up to the string of level ups
				if (levelUps.compare("") == 0)
				{
					levelUps = newLevelUp;
				}
				else
				{
					levelUps = levelUps + ITEM_SEPARATOR + newLevelUp;
				}

				//store the left over health, stamina and magicka
				scalingWeight = weight - weightNeeded;
				totalHealth = ((health * 10) * scalingWeight) / total;
				totalStamina = ((stamina * 10) * scalingWeight) / total;
				totalMagicka = (10 * scalingWeight) - (totalHealth + totalStamina);
			}

			totalWeight = (totalHealth + totalStamina + totalMagicka) / 10;
		}
		catch (const std::out_of_range& oor)
		{
		}
	}

	//add the left over health, stamina and magicka to outstandingLevel
	std::string totals = std::to_string(totalHealth) + FIELD_SEPARATOR + std::to_string(totalStamina) + FIELD_SEPARATOR + std::to_string(totalMagicka);

	//add the outstanding level to the start of the return string
	std::string levelUpsFinal = totals;
	if (levelUps.compare("") != 0)
	{
		levelUpsFinal = levelUpsFinal + ITEM_SEPARATOR + levelUps;
	}
	return levelUpsFinal;
}

//Returns true if there is another level up and sets the health,stamina and magicka values
bool PluginFunctions::isNthLevelUp(std::string levelUpsString, int n)
{

	if (levelUpsString.compare("") == 0)
	{
		return FALSE;
	}
	std::vector<std::string> levelUps = split(levelUpsString, ITEM_SEPARATOR);
	if (n >= levelUps.size())
	{
		return FALSE;
	}
	return TRUE;
}

//Returns the health, stamina or magicka component of the given level up
int PluginFunctions::getLevelComponent(std::string levelUpsString, int n, std::string type)
{
	std::vector<std::string> levelUps = split(levelUpsString, ITEM_SEPARATOR);
	std::vector<std::string> levelUpComponents = split(levelUps.at(n), FIELD_SEPARATOR);
	int levelComponent = std::stoi(levelUpComponents.at(2));
	if (type == "H")
	{
		levelComponent = std::stoi(levelUpComponents.at(0));
	}
	else if (type == "S")
	{
		levelComponent = std::stoi(levelUpComponents.at(1));
	}
	return levelComponent;
}

//Returns the outstanding level as a string
std::string PluginFunctions::getOutstandingLevel(std::string levelUpsString)
{
	std::vector<std::string> levelUps = split(levelUpsString, ITEM_SEPARATOR);
	return levelUps.at(0);
}

//Makes a service call to fetch workouts
int PluginFunctions::startNormalFetch(std::string gameID, std::string username)
{
	debug.entry();
	ConfigHandler config;
	std::string fromDate = std::to_string(config.getConfigProperty("lastSyncDate"));
	std::string toDate = std::to_string(currentDate());
	if (config.getConfigProperty("startDate") == 0)
	{
		toDate = std::to_string(currentDate());
		config.setConfigProperty("startDate", weekHandler.getStartOfDay(currentDate()));
	}
	makeServiceCall("NORMAL", username, fromDate, toDate);
	debug.exit();
	return 345;
}

//Starts the poll for new workouts when the user requests a check
bool PluginFunctions::startForceFetch(std::string gameID, std::string username)
{
	//Start the headless browser by making the force fetch request
	makeServiceCall("FORCE_FETCH", username, "0", "0");

	if (rawData.getResponseCode() == "200")
	{
		return true;
	}
	return false;
}

//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
std::string PluginFunctions::getWorkoutsString(int level)
{
	debug.entry();
	rawData.refresh();
	std::string workouts;
	if (config.getConfigProperty("startDate") == 0)
	{
		int startDate = (currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY;
		config.setConfigProperty("startDate", startDate);
		if (rawData.getWorkoutCount() > 0)
		{
			workouts = "Prior Workout";
		}
	}
	else
	{
		workouts = updateWeeks(level).c_str();
	}
	debug.exit();
	return workouts;
}

//Returns the number of workouts in the raw data file
int PluginFunctions::getRawDataWorkoutCount()
{
	rawData.refresh();
	return rawData.getWorkoutCount();
}

//Allows papyrus to clear the debug
void PluginFunctions::clearDebug()
{
	debug.clear();
}

//Checks if the given username is valid
bool PluginFunctions::validUsername(std::string gameID, std::string username)
{
	makeServiceCall("NORMAL", username, "0", "0");
	if (rawData.getResponseCode() == "404")
	{
		return false;
	}
	return true;
}

//Returns a shortened username to fit in the menu screen
std::string PluginFunctions::getShortenedUsername(std::string username)
{
	std::string shortenedUsername = username;
	if (username.length() > 10)
	{
		shortenedUsername = username.substr(0, 8) + "...";
	}
	return shortenedUsername.c_str();
}

//Virtually presses the given key
void PluginFunctions::pressKey(std::string key)
{
	INPUT input;
	WORD vkey = 0x57;
	if (std::string("VK_TAB").compare(key) == 0)
	{
		vkey = VK_TAB;
	}
	else if (std::string("VK_UP").compare(key) == 0)
	{
		vkey = 0x57;
	}
	else
	{
	}

	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;

	//Press Key
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));

	//Release Key
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}