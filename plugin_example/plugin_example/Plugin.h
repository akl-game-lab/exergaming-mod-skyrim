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

	//Returns the best week between the creation date of the calling save and now
	UInt32 getBestWeek(StaticFunctionTag* base, BSFixedString creationDate);

	//Returns the given weeks workouts as a string (format is "W,H,S,M;W,H,S,M...")
	BSFixedString getNthWeeksWorkouts(StaticFunctionTag* base, UInt32 weekNumber);

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	BSFixedString getLevelUpsAsString(StaticFunctionTag* base, BSFixedString outstandingLevel, BSFixedString workoutsString);

	//Returns true if there is a level up
	bool isNthLevelUp(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n);

	//Returns the health, stamina or magicka component of the given level up
	UInt32 getLevelComponent(StaticFunctionTag* base, BSFixedString levelUpsString, UInt32 n, BSFixedString type);

	//Returns the outstanding level weight
	float getOutstandingWeight(BSFixedString outstandingLevel);

	//Returns the outstanding level as a string
	BSFixedString getOutstandingLevel(StaticFunctionTag* base, BSFixedString levelUpsString);

	/**********************************************************************************************************
	*	Register
	*/

	//Registers functions for use in a papyrus script
	bool RegisterFuncs(VMClassRegistry* registry);
}