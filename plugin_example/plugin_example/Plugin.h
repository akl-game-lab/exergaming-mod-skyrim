#include "skse/PapyrusNativeFunctions.h"
#include "skse/PapyrusVM.h"

namespace plugin
{	
	/**********************************************************************************************************
	*	Functions
	*/

	//Returns the current date to the calling papyrus script
	UInt32 currentDate(StaticFunctionTag* base);

	//Checks if the current save is old
	bool isOldSave(StaticFunctionTag* base, BSFixedString creationDate);

	//Returns workouts logged between the given date to now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString fetchWorkouts(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName, UInt32 level);

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsFromBestWeek(StaticFunctionTag* base, BSFixedString creationDate);

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString);

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n);

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type);

	//Returns the outstanding level as a string
	BSFixedString getOutstandingLevel(StaticFunctionTag* base, BSFixedString levelUpsString);

	//Starts the poll for new workouts when the user requests a check
	void startForceFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username, UInt32 level);

	//Allows papyrus to read the config
	BSFixedString getConfigProperty(StaticFunctionTag* base, BSFixedString propertyName);

	//Allows papyrus to clear the debug
	void clearDebug(StaticFunctionTag* base);

	/**********************************************************************************************************
	*	Register
	*/

	//Registers functions for use in a papyrus script
	bool RegisterFuncs(VMClassRegistry* registry);
}