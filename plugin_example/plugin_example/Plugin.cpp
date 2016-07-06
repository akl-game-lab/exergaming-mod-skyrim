#include "Plugin.h"
#include "PluginLibrary.h"

namespace plugin
{
	/**********************************************************************************************************
	*	Functions
	*/

	//Returns the current date to the calling papyrus script
	UInt32 currentDate(StaticFunctionTag* base)
	{
		time_t t;
		time(&t);
		return t;
	}

	//Checks if the current save is old
	bool isOldSave(StaticFunctionTag* base, UInt32 creationDate)
	{
		int lastSyncDate = config.getConfigProperty("lastSyncDate");
		return creationDate < lastSyncDate;
	}

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsFromBestWeek(StaticFunctionTag* base, UInt32 creationDate)
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
					for (int workoutNumber = 0; workoutNumber < workouts.size(); workoutNumber++)
					{
						json workout(workouts[workoutNumber]);
						__int64 workoutDate = (__int64)workout["workoutDate"];
						//if it was done on a day of the week after that of the creation date add the weight to the total weight for this week
						if (workoutDate >= creationDate && weekHandler.getDayOfWeek(workoutDate) >= weekHandler.getDayOfWeek(creationDate)) {
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
		return bestWeeksWorkouts.c_str();
	}

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString)
	{
		std::string levelUps = "";

		//initialise the level ditribution with the outstanding points
		std::vector<std::string> outstandingLevelFields = split(outstandingLevel.data, FIELD_SEPARATOR);

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

		std::vector<std::string> workouts = split(workoutsString.data, ITEM_SEPARATOR);
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
		return levelUpsFinal.c_str();
	}

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n)
	{
		if (std::string(levelUpsString.data).compare("") == 0)
		{
			return FALSE;
		}
		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);
		if (n >= levelUps.size())
		{
			return FALSE;
		}
		return TRUE;
	}

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type)
	{
		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);
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
	BSFixedString getOutstandingLevel(StaticFunctionTag* base, BSFixedString levelUpsString)
	{
		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);
		return levelUps.at(0).c_str();
	}

	//Makes a service call to fetch workouts
	UInt32 startNormalFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		std::string fromDate = std::to_string(config.getConfigProperty("lastSyncDate"));
		std::string toDate = std::to_string(currentDate(NULL));
		if (config.getConfigProperty("startDate") == 0)
		{
			toDate = std::to_string(currentDate(NULL));
			config.setConfigProperty("startDate", weekHandler.getStartOfDay(currentDate(NULL)));
		}
		makeServiceCall("NORMAL", username.data, fromDate, toDate);
		return 345;
	}

	//Starts the poll for new workouts when the user requests a check
	bool startForceFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		//Start the headless browser by making the force fetch request
		makeServiceCall("FORCE_FETCH", username.data, "0", "0");

		if (rawData.getResponseCode() == "200")
		{
			return true;
		}
		return false;
	}

	//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsString(StaticFunctionTag* base, UInt32 level)
	{
		rawData.refresh();
		std::string workouts;
		if (config.getConfigProperty("startDate") == 0)
		{
			int startDate = (currentDate(NULL) / SECONDS_PER_DAY)*SECONDS_PER_DAY;
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
		return workouts.c_str();
	}

	//Returns the number of workouts in the raw data file
	UInt32 getRawDataWorkoutCount(StaticFunctionTag* base)
	{
		rawData.refresh();
		return rawData.getWorkoutCount();
	}

	//Allows papyrus to clear the debug
	void clearDebug(StaticFunctionTag* base)
	{
		debug.clear();
	}

	//Checks if the given username is valid
	bool validUsername(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		makeServiceCall("NORMAL", username.data, "0", "0");
		if (rawData.getResponseCode() == "404")
		{
			return false;
		}
		return true;
	}

	//Returns a shortened username to fit in the menu screen
	BSFixedString getShortenedUsername(StaticFunctionTag* base, BSFixedString username)
	{
		std::string shortenedUsername = username.data;
		if (std::string(username.data).length() > 10)
		{
			shortenedUsername = std::string(username.data).substr(0, 8) + "...";
		}
		return shortenedUsername.c_str();
	}

	//Virtually presses the given key
	void pressKey(StaticFunctionTag* base, BSFixedString key)
	{

	}

	/**********************************************************************************************************
	*	Register
	*/

	//Registers functions for use in a papyrus script
	bool RegisterFuncs(VMClassRegistry* registry) {

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, UInt32>("currentDate", "PluginScript", plugin::currentDate, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, UInt32>("isOldSave", "PluginScript", plugin::isOldSave, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, UInt32>("getWorkoutsFromBestWeek", "PluginScript", plugin::getWorkoutsFromBestWeek, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("getLevelUpsAsString", "PluginScript", plugin::getLevelUpsAsString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, UInt32>("isNthLevelUp", "PluginScript", plugin::isNthLevelUp, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, UInt32, BSFixedString, UInt32, BSFixedString>("getLevelComponent", "PluginScript", plugin::getLevelComponent, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("getOutstandingLevel", "PluginScript", plugin::getOutstandingLevel, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("startNormalFetch", "PluginScript", plugin::startNormalFetch, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("startForceFetch", "PluginScript", plugin::startForceFetch, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, UInt32>("getWorkoutsString", "PluginScript", plugin::getWorkoutsString, registry));

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, UInt32>("getRawDataWorkoutCount", "PluginScript", plugin::getRawDataWorkoutCount, registry));

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, void>("clearDebug", "PluginScript", plugin::clearDebug, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("validUsername", "PluginScript", plugin::validUsername, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("getShortenedUsername", "PluginScript", plugin::getShortenedUsername, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, void, BSFixedString>("pressKey", "PluginScript", plugin::pressKey, registry));

		return true;
	}
}