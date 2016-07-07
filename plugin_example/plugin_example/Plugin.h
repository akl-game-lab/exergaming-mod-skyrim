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
	bool isOldSave(StaticFunctionTag* base, UInt32 creationDate);

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsFromBestWeek(StaticFunctionTag* base, UInt32 creationDate);

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString);

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n);

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type);

	//Returns the outstanding level as a string
	BSFixedString getOutstandingLevel(StaticFunctionTag* base, BSFixedString levelUpsString);

	//Makes a service call to fetch workouts
	UInt32 startNormalFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName);

	//Starts the poll for new workouts when the user requests a check
	bool startForceFetch(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username);
	
	//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getWorkoutsString(StaticFunctionTag* base, UInt32 level);

	//Returns the number of workouts in the raw data file
	UInt32 getRawDataWorkoutCount(StaticFunctionTag* base);

	//Allows papyrus to clear the debug
	void clearDebug(StaticFunctionTag* base);

	//Checks if the given username is valid
	bool validUsername(StaticFunctionTag* base, BSFixedString gameID, BSFixedString username);

	//Returns a shortened username to fit in the menu screen
	BSFixedString getShortenedUsername(StaticFunctionTag* base, BSFixedString username);

	//Virtually presses the given key
	void pressKey(BSFixedString key);

	/**********************************************************************************************************
	*	Register
	*/

	//Registers functions for use in a papyrus script
	bool RegisterFuncs(VMClassRegistry* registry);
}