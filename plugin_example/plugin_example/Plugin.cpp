#include "Plugin.h"
#include "PluginFunctions.h"
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
	bool isOldSave(StaticFunctionTag* base, UInt32 creationDate)
	{
		return isOldSave(creationDate);
	}

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsFromBestWeek(StaticFunctionTag* base, UInt32 creationDate)
	{
		return getWorkoutsFromBestWeek(creationDate).c_str();
	}

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString)
	{
		return getLevelUpsAsString(outstandingLevel.data, workoutsString.data).c_str();
	}

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n)
	{
		return isNthLevelUp(levelUpsString.data, n);
	}

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type)
	{
		return getLevelComponent(levelUpsString.data,n,type.data);
	}

	//Returns the outstanding level as a string
	BSFixedString getOutstandingLevel(StaticFunctionTag* base, BSFixedString levelUpsString)
	{
		return getOutstandingLevel(levelUpsString.data).c_str();
	}

	//Makes a service call to fetch workouts
	void startNormalFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		startNormalFetch(gameID.data,username.data);
	}

	//Starts the poll for new workouts when the user requests a check
	bool startForceFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		return startForceFetch(gameID.data,username.data);
	}

	//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsString(StaticFunctionTag* base, UInt32 level)
	{
		return getWorkoutsString(level).c_str();
	}

	//Returns the number of workouts in the raw data file
	UInt32 getRawDataWorkoutCount(StaticFunctionTag* base)
	{
		return getRawDataWorkoutCount();
	}

	//Allows papyrus to clear the debug
	void clearDebug(StaticFunctionTag* base)
	{
		clearDebug();
	}

	//Checks if the given username is valid
	bool validUsername(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username)
	{
		return validUsername(gameID.data,username.data);
	}

	//Returns a shortened username to fit in the menu screen
	BSFixedString getShortenedUsername(StaticFunctionTag* base, BSFixedString username)
	{
		return getShortenedUsername(username.data).c_str();
	}

	//Virtually presses the given key
	void pressKey(StaticFunctionTag* base, BSFixedString key)
	{
		pressKey(std::string(key.data));
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
			new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString>("startNormalFetch", "PluginScript", plugin::startNormalFetch, registry));

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