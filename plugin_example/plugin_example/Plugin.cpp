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
		time_t t;
		time(&t);
		return t;
	}

	//Checks if the current save is old
	bool isOldSave(StaticFunctionTag* base, BSFixedString creationDate)
	{
		ConfigHandler config;
		std::string lastSyncDate = config.getConfigProperty("lastSyncDate");
		return std::stoi(creationDate.data) < std::stoi(lastSyncDate);
	}

	//Returns workouts logged between the given date to now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString fetchWorkouts(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName) 
	{
		clearDebug();
		clearRawData();
		ConfigHandler config;

		//Take the arguments and put them into a set of parameters for the executable
		std::string sGameID(gameID.data);
		std::string sUserName(userName.data);
		std::string sFromDate("0"/*config.getConfigProperty("Last_Sync_Date")*/);
		std::string toDate = std::to_string(currentDate(NULL)) + "100";
		writeToDebug(toDate);

		std::string exeParams = sGameID + " " + sUserName + " " + sFromDate + " " + toDate;

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
		ShExecInfo.lpFile = swExePath;
		ShExecInfo.lpParameters = swExeParams;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOWMINNOACTIVE;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		return updateWeeks().c_str();
	}

	//Returns the best week between the creation date of the calling save and now
	UInt32 getBestWeek(StaticFunctionTag* base, BSFixedString creationDate)
	{
		//get the current wee for testing
		return getWeekNumber(currentDate(NULL));
	}

	//Returns the given weeks workouts as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getNthWeeksWorkouts(StaticFunctionTag* base, UInt32 weekNumber)
	{
		std::string workouts = "";

		//get the given week
		WeekHandler weekHandler;

		for (int workoutNumber = 0; workoutNumber < weekHandler.getWorkoutCountForWeek(weekNumber); workoutNumber++)
		{
			if (workoutNumber > 0)
			{
				workouts += ";";
			}

			std::string weightString = weekHandler.getWorkoutProperty(weekNumber,workoutNumber,"weight");
			std::string healthString = weekHandler.getWorkoutProperty(weekNumber, workoutNumber, "health");
			std::string staminaString = weekHandler.getWorkoutProperty(weekNumber, workoutNumber, "stamina");
			std::string magickaString = weekHandler.getWorkoutProperty(weekNumber, workoutNumber, "magicka");

			std::string newWorkoutString = weightString + FIELD_SEPARATOR + healthString + FIELD_SEPARATOR + staminaString + FIELD_SEPARATOR + magickaString;
			workouts += newWorkoutString;
		}

		return workouts.c_str();
	}

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString)
	{
		std::string levelUps = "";
		
		//initialise the level ditribution with the outstanding points
		std::vector<std::string> outstandingLevelFields = split(outstandingLevel.data, FIELD_SEPARATOR);

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
				writeToDebug("out_of_range error for outstandingLevelFields access.");
			}
		}


		std::vector<std::string> workouts = split(workoutsString.data, ITEM_SEPARATOR);
		writeToDebug(std::to_string(workouts.size()) + " workout(s) found.");
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

				writeToDebug("totalHealth = " + std::to_string(totalHealth));
				writeToDebug("totalStamina = " + std::to_string(totalStamina));
				writeToDebug("totalMagicka = " + std::to_string(totalMagicka));
				writeToDebug("weightNeeded = " + std::to_string(weightNeeded));
				writeToDebug("weight = " + std::to_string(weight));

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
					if (levelUps == "")
					{
						levelUps = newLevelUp;
					}
					else
					{
						levelUps = newLevelUp + ITEM_SEPARATOR + levelUps;
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
				writeToDebug("out_of_range error for workout access. Invalid workout [" + workout + "]");
			}
		}
		//add the left over health, stamina and magicka to outstandingLevel
		std::string totals = std::to_string(totalHealth) + FIELD_SEPARATOR + std::to_string(totalStamina) + FIELD_SEPARATOR + std::to_string(totalMagicka);

		outstandingLevel = totals.c_str();
		return levelUps.c_str();
	}

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n)
	{
		writeToDebug("levelUpsString : [" + std::string(levelUpsString.data) + "]");

		if (std::string(levelUpsString.data) == "")
		{
			writeToDebug("No more level ups.");
			return FALSE;
		}

		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);

		if (n >= levelUps.size())
		{
			writeToDebug("No more level ups.");
			return FALSE;
		}

		writeToDebug(std::to_string(levelUps.size()) + " level up(s) found.");
		return TRUE;
	}

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type)
	{
		std::vector<std::string> levelUps = split(levelUpsString.data, ITEM_SEPARATOR);
		std::vector<std::string> levelUpComponents = split(levelUps.at(n), FIELD_SEPARATOR);
		if (type == "H")
		{
			return std::stoi(levelUpComponents.at(0));
		}
		else if (type == "S")
		{
			return std::stoi(levelUpComponents.at(1));
		}
		return std::stoi(levelUpComponents.at(2));
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
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("fetchWorkouts", "PluginScript", plugin::fetchWorkouts, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, UInt32, BSFixedString>("getBestWeek", "PluginScript", plugin::getBestWeek, registry));

		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, BSFixedString, UInt32>("getNthWeeksWorkouts", "PluginScript", plugin::getNthWeeksWorkouts, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("getLevelUpsAsString", "PluginScript", plugin::getLevelUpsAsString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, UInt32>("isNthLevelUp", "PluginScript", plugin::isNthLevelUp, registry));

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, UInt32, BSFixedString, UInt32, BSFixedString>("getLevelComponent", "PluginScript", plugin::getLevelComponent, registry));

		return true;

	}
}