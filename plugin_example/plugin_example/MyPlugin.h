#include "skse/PapyrusNativeFunctions.h"

namespace MyPluginNamespace
{
	float fetchXml(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName, BSFixedString lastSyncDate);

	BSFixedString currentDate(StaticFunctionTag* base);

	bool isFirstWeekCompleted(StaticFunctionTag* base, BSFixedString firstImportDate, BSFixedString workoutDate);

	UInt32 dayOfTheWeek(StaticFunctionTag* base, BSFixedString firstImportDate, BSFixedString workoutDate);

	bool RegisterFuncs(VMClassRegistry* registry);
}
