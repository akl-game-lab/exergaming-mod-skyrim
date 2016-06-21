#include "Plugin.h"
#include "Helpers.h"

namespace plugin
{
	/**********************************************************************************************************
	*	Functions
	*/

	//Returns the current date to the calling papyrus script
	UInt32 currentDate(StaticFunctionTag* base)
	{
		return currentDate();
	}

	//Checks if the current save is old
	bool isOldSave(StaticFunctionTag* base, BSFixedString creationDate)
	{
		std::string lastSyncDate = config.getConfigProperty("lastSyncDate");
		return _atoi64(creationDate.data) < _atoi64(lastSyncDate.c_str());
	}

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsFromBestWeek(StaticFunctionTag* base, UInt32 creationDate)
	{
		debug.write(ENTRY, "getWorkoutsFromBestWeek()");
		return weekHandler.getWorkoutsFromBestWeek(creationDate).c_str();
		debug.write(EXIT, "getWorkoutsFromBestWeek()");
	}

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString)
	{
		debug.write(ENTRY, "getLevelUpsAsString()");
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

				for (int i = 0; i < workoutFields.size(); i++)
				{
					debug.write(WRITE, "Field(" + std::to_string(i) + ") : " + workoutFields.at(i));
				}

				int weight = std::stoi(workoutFields.at(WEIGHT));
				int health = std::stoi(workoutFields.at(HEALTH));
				int stamina = std::stoi(workoutFields.at(STAMINA));
				int magicka = std::stoi(workoutFields.at(MAGICKA));
				int total = health + stamina + magicka;

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

					debug.write(WRITE, "newLevelUp : " + newLevelUp);

					//add the new level up to the string of level ups
					if (levelUps == "")
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
		levelUps = totals + ITEM_SEPARATOR + levelUps;

		debug.write(WRITE, "levelUps : " + levelUps);

		BSFixedString levelUpsBS = levelUps.c_str();

		debug.write(EXIT, "getLevelUpsAsString()");
		return levelUpsBS;
	}

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n)
	{
		debug.write(ENTRY, "isNthLevelUp()");
		if (std::string(levelUpsString.data) == "")
		{
			debug.write(EXIT, "isNthLevelUp()");
			return FALSE;
		}
		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);
		if (n >= levelUps.size())
		{
			debug.write(EXIT, "isNthLevelUp()");
			return FALSE;
		}
		debug.write(EXIT, "isNthLevelUp()");
		return TRUE;
	}

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type)
	{
		debug.write(ENTRY, "getLevelComponent()");
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
		debug.write(EXIT, "getLevelComponent()");
		return levelComponent;
	}

	//Returns the outstanding level as a string
	BSFixedString getOutstandingLevel(StaticFunctionTag* base, BSFixedString levelUpsString)
	{
		debug.write(ENTRY, "getOutstandingLevel()");
		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);
		BSFixedString outstandingLevel = levelUps.at(0).c_str();
		debug.write(EXIT, "getOutstandingLevel()");
		return outstandingLevel;
	}

	//Makes a service call to fetch workouts
	void startNormalFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		debug.write(ENTRY, "startNormalFetch()");
		ConfigHandler config;
		std::string fromDate = config.getConfigProperty("lastSyncDate");
		std::string toDate = std::to_string(currentDate());
		if (_atoi64(config.getConfigProperty("startDate").c_str()) == 0)
		{
			toDate = std::to_string((currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY);
			config.setConfigProperty("startDate", toDate);
		}
		makeServiceCall("NORMAL", username.data, fromDate, toDate);
		debug.write(EXIT, "startNormalFetch()");
	}

	//Starts the poll for new workouts when the user requests a check
	bool startForceFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		debug.write(ENTRY, "startForceFetch");

		//Start the headless browser by making the force fetch request
		makeServiceCall("FORCE_FETCH", username.data, "0", "0");

		if (rawData.getResponseCode() == "200")
		{
			return true;
		}
		debug.write(EXIT, "startForceFetch");
		return false;
	}

	//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsString(StaticFunctionTag* base, UInt32 level)
	{
		debug.write(ENTRY, "getWorkoutsString()");
		rawData.refresh();
		BSFixedString workouts;
		if (_atoi64(config.getConfigProperty("startDate").c_str()) == 0)
		{
			std::string startDate = std::to_string((currentDate() / SECONDS_PER_DAY)*SECONDS_PER_DAY);
			config.setConfigProperty("startDate", startDate);
			if (rawData.getWorkoutCount() > 0)
			{
				workouts = "Prior Workout";
			}
			debug.write(WRITE, "firstFetch");
		}
		else
		{
			workouts = updateWeeks(level).c_str();
		}
		debug.write(EXIT, "getWorkoutsString()");
		return workouts;
	}

	//Returns the number of workouts in the raw data file
	UInt32 getRawDataWorkoutCount(StaticFunctionTag* base)
	{
		rawData.refresh();
		return rawData.getWorkoutCount();
	}

	//Allows papyrus to read the config
	BSFixedString getConfigProperty(StaticFunctionTag* base, BSFixedString propertyName)
	{
		return config.getConfigProperty(propertyName.data).c_str();
	}

	//Allows papyrus to clear the debug
	void clearDebug(StaticFunctionTag* base)
	{
		debug.clear();
	}

	//Checks if the given username is valid
	bool validUsername(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		makeServiceCall("NORMAL", username.data, "0","0");
		if (rawData.getResponseCode() == "404")
		{
			return false;
		}
		return true;
	}

	//Returns a shortened username to fit in the menu screen
	BSFixedString getShortenedUsername(StaticFunctionTag* base, BSFixedString username)
	{
		std::string usernameString = username.data;
		std::string shortenedUsername = usernameString;
		if (usernameString.length() > 10)
		{
			shortenedUsername = usernameString.substr(0, 8) + "...";
		}
		debug.write(WRITE,shortenedUsername);
		return shortenedUsername.c_str();
	}

	/**********************************************************************************************************
	*	Register
	*/

	//Registers functions for use in a papyrus script
	bool RegisterFuncs(VMClassRegistry* registry) {

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, UInt32>("currentDate", "PluginScript", plugin::currentDate, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, BSFixedString>("isOldSave", "PluginScript", plugin::isOldSave, registry));

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
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString>("startNormalFetch", "PluginScript", plugin::startNormalFetch, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("startForceFetch", "PluginScript", plugin::startForceFetch, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, UInt32>("getWorkoutsString", "PluginScript", plugin::getWorkoutsString, registry));

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, UInt32>("getRawDataWorkoutCount", "PluginScript", plugin::getRawDataWorkoutCount, registry));
		
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("getConfigProperty", "PluginScript", plugin::getConfigProperty, registry));

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, void>("clearDebug", "PluginScript", plugin::clearDebug, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("validUsername", "PluginScript", plugin::validUsername, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, BSFixedString>("getShortenedUsername", "PluginScript", plugin::getShortenedUsername, registry));

		return true;
	}
}