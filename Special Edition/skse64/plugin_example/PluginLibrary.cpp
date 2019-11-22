#include "PluginLibrary.h"
#include <Windows.h>\

#include <iostream>
#include <fstream>





/**********************************************************************************************************
*	Helpers
*/
//Copies the data from file and writes it into debug.txt
int PluginFunctions::copyDataFromFile(char* filename) {
	FILE *fptr1, *fptr2;
	char c;
	fptr1 = fopen(filename, "r");
	c = fgetc(fptr1);
	std::string contents = "";
	while (c != EOF)
	{
		contents = contents + c;
		c = fgetc(fptr1);
	}
	debug.write(contents);
	fclose(fptr1);
	return 0;
}

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

//Returns how many workouts were logged in the given day
int PluginFunctions::getWorkoutsInDay(__int64 date) {
	int workoutCount = rawData.getWorkoutCount();
	int workoutsinday = 0;
	for (int workoutNumber = 0; workoutNumber < workoutCount; workoutNumber++)
	{
		json workout = rawData.getWorkout(workoutNumber);
		if (weekHandler.getStartOfDay((__int64)workout["workoutDate"]) == weekHandler.getStartOfDay(date)) {
			workoutsinday = workoutsinday + 1;
		}
	}
	return workoutsinday;
}

//Returns a float representation of the number of levels gained from the workout passed to the method
float PluginFunctions::getWeightForWorkout(json workout, int level)
{
	// the amount of extra experience (points) needed per specified number of levels needed 
	float levelsGained = 0.0;

	__int64 startDate = config.getConfigProperty("startDate");
	__int64 lastWorkoutDate = config.getConfigProperty("lastWorkoutDate");
	__int64 firstWorkoutDate = config.getConfigProperty("firstWorkoutDate");

	int workoutCount = config.getConfigProperty("workoutCount");
	int weeksWorkedOut = weekHandler.getWeekCount(); 
	int avgPointsPerWorkout = config.getConfigProperty("avgPointsPerWorkout");
	int totalPoints = config.getConfigProperty("totalPoints");
	int workoutsThisWeek = config.getConfigProperty("workoutsThisWeek");

	float workoutPoints = (int)workout["health"] + (int)workout["stamina"] + (int)workout["magicka"];
	if (firstWorkoutDate == 0 && workoutCount == 0 && (__int64)workout["workoutDate"] > startDate)
	{
		firstWorkoutDate = (__int64)workout["workoutDate"];
		debug.write("setting first workout date");
		config.setConfigProperty("firstWorkoutDate", firstWorkoutDate);
	}

	int workoutsWeek = getWeekForWorkout(firstWorkoutDate, (__int64)workout["workoutDate"]);
	int lastWorkoutsWeek = getWeekForWorkout(firstWorkoutDate, lastWorkoutDate);

	//if workout is for before the player synced their account
	
	if ((__int64)workout["workoutDate"] < startDate|| (__int64)workout["workoutDate"] < firstWorkoutDate)
	{
		config.setConfigProperty("lastSyncDate", currentDate());
		return 0.0;
	}
	else
	{
		totalPoints = totalPoints + workoutPoints;
		workoutCount = workoutCount + 1;
		
		if ((__int64)workout["workoutDate"] == startDate)
		{
			workoutsWeek = 1;
		}

		if (workoutsWeek == 1)
		{
			if (weeksWorkedOut == 0)
			{
				weeksWorkedOut = 1;
			}
			levelsGained = 1.0;
		}
		else if (workoutsWeek == lastWorkoutsWeek)
		{
			/*When the workout is part of the current week*/
			lastWorkoutDate = workout["workoutDate"];
			workoutsThisWeek = workoutsThisWeek + 1;
		}
		else if (workoutsWeek > lastWorkoutsWeek)
		{
				/*When the workout is in a new week*/
				lastWorkoutDate = workout["workoutDate"];
				weeksWorkedOut = weeksWorkedOut + 1;
				workoutsThisWeek = 1;
		}
		debug.write("levels gained: " + std::to_string(levelsGained));
		if (levelsGained == 0.0)
		{
			debug.write("levels gained: " + std::to_string(levelsGained));
			avgPointsPerWorkout = (totalPoints / workoutCount);
			float avgWorkoutsPerWeek = ((float)(workoutCount - workoutsThisWeek) / (weeksWorkedOut - 1));
			debug.write("weekcount: " + std::to_string(weekHandler.getWeekCount()));
			if (weeksWorkedOut > 4) {
				int fourth = weekHandler.getWorkoutCount(weeksWorkedOut - 1);
				int third = weekHandler.getWorkoutCount(weeksWorkedOut - 2);
				int second = weekHandler.getWorkoutCount(weeksWorkedOut - 3);
				int first = weekHandler.getWorkoutCount(weeksWorkedOut - 4);
				ESTIMATED_LEVELS_PER_WEEK = (fourth + third + second + first) / 4;
				debug.write("total workouts in last month: " + std::to_string(first + second + third + fourth));
			}
			if (ESTIMATED_LEVELS_PER_WEEK > 6) {
				ESTIMATED_LEVELS_PER_WEEK = 6;
			}
			if (ESTIMATED_LEVELS_PER_WEEK < 2) {
				ESTIMATED_LEVELS_PER_WEEK = 2;
			}
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
	debug.write("levels gained: " + std::to_string(levelsGained));
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

void PluginFunctions::makeCall(std::string type, std::string url) {
	rawData.clear();

	std::string exeParams = type + " " + url;
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
	std::string workoutsString = "";
	int health = 0, stamina = 0, magicka = 0;
	int workoutCount = rawData.getWorkoutCount();
	bool workoutLoggedPrior = true;
	__int64 startDate = config.getConfigProperty("startDate"); 
	__int64 endDate = startDate + SECONDS_PER_WEEK;
	int workouts = 0;
	int daycount = 0;
	for (int day = startDate; day < endDate; day += SECONDS_PER_DAY) {
		int count = getWorkoutsInDay(day);
		workouts = workouts + count;
		if (count != 0) {
			daycount += 1;
		}
	}
	float levelWeight = (float)daycount / (float)workouts;
	debug.write("weight = " + std::to_string(daycount) + "/" + std::to_string(workouts) + "=" +std::to_string(levelWeight));
	//for each workout
	for (int workoutNumber = 0; workoutNumber < workoutCount; workoutNumber++)
	{
		json workout = rawData.getWorkout(workoutNumber);
		//adjust the config to account for the new workout and gets the workouts weight
		float weight = getWeightForWorkout(workout, level);
		workout["weight"] = weight;
		if ((__int64)workout["workoutDate"] <= endDate) {
			debug.write("firstweek");
			workout["weight"] = levelWeight;
		}
		debug.write("weight = " + std::to_string(weight));
		debug.write("checking workout weight");
		if (weight > 0)
		{
			debug.write("adding workout to weeks");
			workoutLoggedPrior = false;
			weekHandler.addWorkout(workout);
			debug.write("workout added to weeks");
			if (workoutNumber > 0)
			{
				workoutsString = workoutsString + ";";
			}

			workoutsString = workoutsString + workoutToString(workout);
		}
	}
	if (workoutLoggedPrior)
	{
		workoutsString = "Workout Logged Prior";
	}
	return workoutsString;
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

//Checks if the current save is older than the most recent sync time + 60 seconds
bool PluginFunctions::isOldSave(int creationDate)
{
	int lastSyncDate = config.getConfigProperty("lastSyncDate");
	return creationDate < lastSyncDate && creationDate > 0;
}

//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
std::string PluginFunctions::getWorkoutsFromBestWeek(__int64 creationDate)
{
	debug.write("getWorkoutsFromBestWeek");
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
	debug.write(bestWeeksWorkouts);
	return bestWeeksWorkouts;
}

//Returns a string representation of the levels gained(format is "H,S,M;H,S,M...")
std::string PluginFunctions::getLevelUpsAsString(std::string outstandingLevel, std::string workoutsString)
{
	debug.write("getLevelUpsAsString for " + workoutsString);
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
	debug.write(std::to_string(outstandingWeight));
	if (weekHandler.getWeekCount() <= 1) {
		outstandingWeight = 0;
		outstandingHealth = 0;
		outstandingStamina = 0;
		outstandingMagicka = 0;
	}
	debug.write(std::to_string(outstandingWeight));
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
	debug.write("Exiting normal fetch\n\nReturning response code:" + std::to_string(rawData.getResponseCode()));
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
	debug.write("getting workouts string");
	rawData.refresh();
	std::string workouts;
	try {
		debug.write("Getting start date from config");
		if (config.getConfigProperty("startDate") == 0)
		{
			debug.write("Getting start of current day");
			int startDate = (currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY;
			debug.write("setting start date");
			config.setConfigProperty("startDate", startDate);
			debug.write("setting last sync date");
			config.setConfigProperty("lastSyncDate", currentDate());
			if (rawData.getWorkoutCount() > 0)
			{
				debug.write("setting workouts");
				workouts = "Prior Workout";
			}
		}
		else
		{
			debug.write("updating weeks");
			workouts = updateWeeks(level).c_str();
		}
	}
	catch (...) 
	{
		debug.write("error in getting workouts string");
	}

	debug.write("returning workouts string(" + workouts + ")");
	return workouts;
}

void PluginFunctions::runDiagnosis(std::string email) {
	debug.write("Config.json:");
	copyDataFromFile("Config.json");
	debug.write("Raw Data:");
	copyDataFromFile("Raw_Data.json");
	debug.write("running diagnosis");
	std::string ip_address = "130.216.216.167";
	debug.write("pinging " + ip_address);
	if (system(std::string("ping " + ip_address + ">nul").c_str()) == 0) {
		debug.write("ping successful");
	}
	else {
		debug.write("ping unsuccessful");
	}
	debug.write("Checking if webservicetest is available");

	rawData.getDefaultData();
	json empty = {};
	makeCall("DEBUG", "http://httpbin.org/get");
	json data = rawData.getData();
	if (data != empty && data != "null") {
		debug.write("webservicetest is available");
	}
	else {
		debug.write("webservicetest is unavailable");
	}
	Sleep(1000);
	debug.write("checking if email is valid");
	rawData.clear();
	if (validUsername("Skyrim", email)) {
		debug.write("email is valid");
	}
	else {
		debug.write("email is invalid");
	}
	Sleep(1000);
	debug.write("Attempting to reach default email");
	int response_code = startForceFetch("Skyrim", "paulralph@gmail.com");
	if (response_code == 200) {
		debug.write(std::to_string(response_code) + " server reached");
	}
	else {
		debug.write(std::to_string(response_code) + " cannot reach server");
	}
	//**********

	//***********
	Sleep(2000);
	debug.write("checking " + email);
	makeServiceCall("NORMAL", email, "0", std::to_string(currentDate()));
	if (rawData.getResponseCode() == 200) {
		debug.write("email is in database");
		if (rawData.getWorkoutCount() > 0) {
			debug.write("email contains workouts");
		}
		else {
			debug.write("email contains no workouts");
		}
	}
	else {
		debug.write("email is not in database");
	}

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
	debug.write(std::to_string(rawData.getResponseCode()));
	if (rawData.getResponseCode() == 200)
	{
		return true;
	}
	return false;
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

int PluginFunctions::getConfigProperty(std::string propertyname){
	int date = config.getConfigProperty(propertyname);
	return date;
}

//Returns a%b
int PluginFunctions::mod(int a, int b)
{
	return a%b;
}

//Returns a/b
int PluginFunctions::division(int c, int d)
{
	return c/d;
}
