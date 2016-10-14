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
	if (workoutTime - firstTime == 0)
	{
		return 1;
	}
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
	float levelsGained = 0.0;

	__int64 startDate = config.getConfigProperty("startDate");
	__int64 lastWorkoutDate = config.getConfigProperty("lastWorkoutDate");
	__int64 firstWorkoutDate = config.getConfigProperty("firstWorkoutDate");

	int workoutCount = config.getConfigProperty("workoutCount");
	int weeksWorkedOut = config.getConfigProperty("weeksWorkedOut");
	int avgPointsPerWorkout = config.getConfigProperty("avgPointsPerWorkout");
	int totalPoints = config.getConfigProperty("totalPoints");
	int workoutsThisWeek = config.getConfigProperty("workoutsThisWeek");

	float workoutPoints = (int)workout["health"] + (int)workout["stamina"] + (int)workout["magicka"];
	if (firstWorkoutDate == 0 && workoutCount == 0 && (__int64)workout["workoutDate"] > startDate)
	{
		firstWorkoutDate = (__int64)workout["workoutDate"];
		config.setConfigProperty("firstWorkoutDate", firstWorkoutDate);
	}

	int workoutsWeek = getWeekForWorkout(firstWorkoutDate, (__int64)workout["workoutDate"]);
	int lastWorkoutsWeek = getWeekForWorkout(firstWorkoutDate, lastWorkoutDate);

	//if workout is for before the player synced their account
	if ((__int64)workout["workoutDate"] < startDate || (__int64)workout["workoutDate"] < firstWorkoutDate)
	{
		config.setConfigProperty("lastSyncDate", currentDate());
		return levelsGained;
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
			avgPointsPerWorkout = (totalPoints / workoutCount);
			float avgWorkoutsPerWeek = ((float)(workoutCount - workoutsThisWeek) / (weeksWorkedOut - 1));

			levelsGained = ((ESTIMATED_LEVELS_PER_WEEK * workoutPoints) / ((float)avgPointsPerWorkout * avgWorkoutsPerWeek));
			levelsGained = levelsGained / (1.0 + (((float)level / LEVEL_IMPROVEMENT) * EXP_INCREASE_RATE));
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
		float weight = configure(workout, level);
		workout["weight"] = weight;
		float workoutWeight = (float)workout["weight"];
		if (workoutWeight > 0)
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

//Returns a string representation of the levels gained(format is "H,S,M;H,S,M...")
std::string PluginFunctions::getLevelUpsAsString(std::string outstandingLevel, std::string workoutsString)
{
	std::string levelUps = "";

	//initialise the level ditribution with the outstanding points
	std::vector<std::string> outstandingLevelFields = split(outstandingLevel, FIELD_SEPARATOR);

	float outstandingHealth = 0;
	float outstandingStamina = 0;
	float outstandingMagicka = 0;
	float outstandingWeight = 0;

	if (outstandingLevelFields.size() == 3)
	{
		try
		{
			outstandingHealth = std::stof(outstandingLevelFields.at(HEALTH - 1).c_str());
			outstandingStamina = std::stof(outstandingLevelFields.at(STAMINA - 1).c_str());
			outstandingMagicka = std::stof(outstandingLevelFields.at(MAGICKA - 1).c_str());
			outstandingWeight = (outstandingHealth + outstandingStamina + outstandingMagicka);
		}
		catch (const std::out_of_range& oor)
		{
		}
	}

	std::vector<std::string> workouts = split(workoutsString, ITEM_SEPARATOR);
	//loop through the given weeks workouts to see if a level up can be awarded
	std::cout << "\nWC:" + std::to_string(workouts.size());
	for (int i = 0; i < workouts.size(); i++)
	{
		std::string workout = workouts.at(i);
		std::vector<std::string> workoutFields = split(workout, FIELD_SEPARATOR);
		try
		{
			float workoutWeight = std::stof(workoutFields.at(WEIGHT));
			float workoutHealth = std::stof(workoutFields.at(HEALTH));
			float workoutStamina = std::stof(workoutFields.at(STAMINA));
			float workoutMagicka = std::stof(workoutFields.at(MAGICKA));
			float workoutTotal = workoutHealth + workoutStamina + workoutMagicka;

			if (workoutTotal != 0)
			{
				float totalWeight = outstandingWeight + workoutWeight;

				std::cout << "\n\nNEW_WORKOUT";

				std::cout << "\nWW:" + std::to_string(workoutWeight);
				std::cout << "\nWH:" + std::to_string(workoutHealth);
				std::cout << "\nWS:" + std::to_string(workoutStamina);
				std::cout << "\nWM:" + std::to_string(workoutMagicka);
				std::cout << "\nWT:" + std::to_string(workoutTotal);

				//while there is another level up from this workout
				if (totalWeight >= 1)
				{
					while (totalWeight >= 1)
					{
						std::cout << "\nOW:" + std::to_string(outstandingWeight);
						std::cout << "\nOH:" + std::to_string(outstandingHealth);
						std::cout << "\nOS:" + std::to_string(outstandingStamina);
						std::cout << "\nOM:" + std::to_string(outstandingMagicka);
						std::cout << "\nTW:" + std::to_string(totalWeight);
						//Calculate each value using the weight needed (1 - outstandingWeight)
						int levelHealth = round((outstandingHealth + ((workoutHealth / workoutTotal) * (1 - outstandingWeight))) * 10);
						float tempStamina = (outstandingStamina + ((workoutStamina / workoutTotal) * (1 - outstandingWeight))) * 10;
						int levelStamina = round(tempStamina);
						if (levelStamina + levelHealth > 10)
						{
							levelStamina = floor(tempStamina);
						}

						int levelMagicka = (10 - levelHealth) - levelStamina;
						if (levelUps.compare("") != 0)
						{
							levelUps = levelUps + ITEM_SEPARATOR;
						}
						//Add new level to levelUps string
						levelUps = levelUps + std::to_string(levelHealth) + FIELD_SEPARATOR + std::to_string(levelStamina) + FIELD_SEPARATOR + std::to_string(levelMagicka);
						totalWeight = totalWeight - 1;
						outstandingWeight = totalWeight;
						outstandingHealth = (workoutHealth / workoutTotal) * outstandingWeight;
						outstandingStamina = (workoutStamina / workoutTotal) * outstandingWeight;
						outstandingMagicka = abs((outstandingWeight - outstandingHealth) - outstandingStamina);
					}
				}
				else
				{
					std::cout << "\nOW:" + std::to_string(outstandingWeight);
					std::cout << "\nOH:" + std::to_string(outstandingHealth);
					std::cout << "\nOS:" + std::to_string(outstandingStamina);
					std::cout << "\nOM:" + std::to_string(outstandingMagicka);
					std::cout << "\nTW:" + std::to_string(totalWeight);
					outstandingWeight = totalWeight;
					outstandingHealth = outstandingHealth + (workoutHealth / workoutTotal) * workoutWeight;
					outstandingStamina = outstandingStamina + (workoutStamina / workoutTotal) * workoutWeight;
					outstandingMagicka = abs((outstandingWeight - outstandingHealth) - outstandingStamina);
				}
			}
		}
		catch (const std::out_of_range& oor)
		{
		}
	}

	//if there is a level up, append it to after outstanding points, otherwise turn outstanding points to string
	if (levelUps.compare("") != 0)
	{
		levelUps = std::to_string(outstandingHealth) + FIELD_SEPARATOR + std::to_string(outstandingStamina) + FIELD_SEPARATOR + std::to_string(outstandingMagicka) + ITEM_SEPARATOR + levelUps;
	}
	else 
	{
		levelUps = std::to_string(outstandingHealth) + FIELD_SEPARATOR + std::to_string(outstandingStamina) + FIELD_SEPARATOR + std::to_string(outstandingMagicka);
	}
	DebugHandler debug;
	debug.write(levelUps);
	int levelCount = split(levelUps, ';').size();
	debug.write(std::to_string(levelCount));
	return levelUps;
}

//Returns true if there is another level up and sets the health,stamina and magicka values
//Returns true if there is no Nth level
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
	float levelComponent = std::stof(levelUpComponents.at(2));
	if (type == "H")
	{
		levelComponent = std::stof(levelUpComponents.at(0));
	}
	else if (type == "S")
	{
		levelComponent = std::stof(levelUpComponents.at(1));
	}
	if (n == 0)
	{
		levelComponent = levelComponent * 100;
	}
	return (int)levelComponent;
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
	rawData.clear();
	ConfigHandler config;
	std::string fromDate = std::to_string(config.getConfigProperty("lastSyncDate"));
	std::string toDate = std::to_string(currentDate());
	if (config.getConfigProperty("startDate") == 0)
	{
		config.setConfigProperty("startDate", weekHandler.getStartOfDay(currentDate()));
		config.setConfigProperty("lastSyncDate", weekHandler.getStartOfDay(currentDate()));
	}
	makeServiceCall("NORMAL", username, fromDate, toDate);
	return rawData.getResponseCode();
}

//Starts the poll for new workouts when the user requests a check
int PluginFunctions::startForceFetch(std::string gameID, std::string username)
{
	//Start the headless browser by making the force fetch request
	makeServiceCall("FORCE_FETCH", username, "0", std::to_string(currentDate()));
	return rawData.getResponseCode();
}

//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
std::string PluginFunctions::getWorkoutsString(int level)
{
	rawData.refresh();
	std::string workouts;
	if (config.getConfigProperty("startDate") == 0)
	{
		int startDate = (currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY;
		config.setConfigProperty("startDate", startDate);
		config.setConfigProperty("lastSyncDate", startDate);
		if (rawData.getWorkoutCount() > 0)
		{
			workouts = "Prior Workout";
		}
	}
	else
	{
		workouts = updateWeeks(level).c_str();
	}
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
	makeServiceCall("NORMAL", username, "0", std::to_string(currentDate()));
	if (rawData.getResponseCode() == 404)
	{
		return false;
	}
	return true;
}

//Returns a shortened username to fit in the menu screen
std::string PluginFunctions::getShortenedUsername(std::string username)
{
	std::string shortenedUsername = username;
	if (username.length() > 11)
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

//Gets the number of exercise.com points needed to level up
int PluginFunctions::getPointsToNextLevel(float outstandingWeight)
{
	int avgPointsPerWorkout = config.getConfigProperty("avgPointsPerWorkout");
	int workoutCount = config.getConfigProperty("workoutCount");
	int weeksWorkedOut = config.getConfigProperty("weeksWorkedOut");
	int workoutsThisWeek = config.getConfigProperty("workoutsThisWeek");
	float avgWorkoutsPerWeek = ((float)(workoutCount - workoutsThisWeek) / (weeksWorkedOut - 1));
	int pointsToNextLevel = round((100.0f - outstandingWeight)*avgPointsPerWorkout*(float(avgWorkoutsPerWeek)/ESTIMATED_LEVELS_PER_WEEK));
	return (pointsToNextLevel / 100);
}

//Updates the config file to match the config object
void PluginFunctions::updateConfig()
{
	config.save();
}

//Returns a%b
int PluginFunctions::mod(int a, int b)
{
	return a%b;
}