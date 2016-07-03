#include "Helpers.h"

namespace plugin
{
	/**********************************************************************************************************
	*	Globals
	*/
	int WEIGHT = 0;
	int HEALTH = 1;
	int STAMINA = 2;
	int MAGICKA = 3;
	char ITEM_SEPARATOR = ';';
	char FIELD_SEPARATOR = ',';

	/**********************************************************************************************************
	*	Functions
	*/

	//Checks if the current save is old
	bool isOldSave(int creationDate)
	{
		std::string lastSyncDate = config.getConfigProperty("lastSyncDate");
		return creationDate < _atoi64(lastSyncDate.c_str());
	}

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	std::string getWorkoutsFromBestWeek(time_t creationDate)
	{
		int weekCount = weekHandler.getWeekCount();
		float bestWeeksWeight = 0;
		std::string bestWeeksWorkouts = "0,0,0,0";

		if (weekCount != 0) {
			for (int weekNumber = 0; weekNumber < weekCount; weekNumber++)
			{
				time_t weekStartDate = weekHandler.getWeekStart(weekNumber);

				//if the week in frame is after the creation date or is the week the save was made
				if (weekHandler.getWeekStart(weekNumber) > creationDate ||
					(weekHandler.getWeekStart(weekNumber) < creationDate && creationDate - weekHandler.getWeekStart(weekNumber) < SECONDS_PER_WEEK))
				{
					float thisWeeksWeight = 0;
					std::string thisWeeksWorkouts = "";
					json workouts = weekHandler.getWorkoutsFromWeek(weekNumber);

					//for each workout
					for (int workoutNumber = 0; workoutNumber < workouts.size(); workoutNumber++)
					{
						json workout(workouts[workoutNumber]);
						//if it was done on a day of the week after that of the creation date add the weight to the total weight for this week
						if ((time_t)workout["workoutDate"] >= creationDate) {
							thisWeeksWeight += workout["weight"];
							if (workoutNumber > 0)
							{
								thisWeeksWorkouts = thisWeeksWorkouts + ";";
							}
							thisWeeksWorkouts = thisWeeksWorkouts + workoutToString(workout);
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
	std::string getLevelUpsAsString(std::string outstandingLevel, std::string workoutsString)
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
	bool isNthLevelUp(std::string levelUpsString, int n)
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
	int getLevelComponent(std::string levelUpsString, int n, std::string type)
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
	std::string getOutstandingLevel(std::string levelUpsString)
	{
		std::vector<std::string> levelUps = split(levelUpsString, ITEM_SEPARATOR);
		return levelUps.at(0);
	}

	//Makes a service call to fetch workouts
	void startNormalFetch(std::string gameID, std::string username)
	{
		debug.entry();
		ConfigHandler config;
		std::string fromDate = config.getConfigProperty("lastSyncDate");
		std::string toDate = std::to_string(currentDate());
		if (_atoi64(config.getConfigProperty("startDate").c_str()) == 0)
		{
			toDate = std::to_string((currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY);
			config.setConfigProperty("startDate", toDate);
		}
		//makeServiceCall("NORMAL", username, fromDate, toDate);
		debug.exit();
	}

	//Starts the poll for new workouts when the user requests a check
	bool startForceFetch(std::string gameID, std::string username)
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
	std::string getWorkoutsString(int level)
	{
		rawData.refresh();
		std::string workouts;
		if (_atoi64(config.getConfigProperty("startDate").c_str()) == 0)
		{
			std::string startDate = std::to_string((currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY);
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
		return workouts;
	}

	//Returns the number of workouts in the raw data file
	int getRawDataWorkoutCount()
	{
		rawData.refresh();
		return rawData.getWorkoutCount();
	}

	//Allows papyrus to clear the debug
	void clearDebug()
	{

	}

	//Checks if the given username is valid
	bool validUsername(std::string gameID, std::string username)
	{
		makeServiceCall("NORMAL", username, "0", "0");
		if (rawData.getResponseCode() == "404")
		{
			return false;
		}
		return true;
	}

	//Returns a shortened username to fit in the menu screen
	std::string getShortenedUsername(std::string username)
	{
		std::string shortenedUsername = username;
		if (username.length() > 10)
		{
			shortenedUsername = username.substr(0, 8) + "...";
		}
		return shortenedUsername.c_str();
	}

	//Virtually presses the given key
	void pressKey(std::string key)
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
}