#include "skse/PapyrusNativeFunctions.h"
#include "skse/PapyrusVM.h"

namespace MyPluginNamespace
{
	UInt32 fetchXml(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName, BSFixedString fromDate);

	BSFixedString currentDate(StaticFunctionTag* base);

	bool isFirstWeekCompleted(StaticFunctionTag* base, BSFixedString firstImportDate, BSFixedString workoutDate);

	UInt32 dayOfTheWeek(StaticFunctionTag* base, BSFixedString firstImportDate, BSFixedString workoutDate);

	BSFixedString getXmlString(StaticFunctionTag* base, BSFixedString fileName, BSFixedString itemType, UInt32 index, BSFixedString field);

	UInt32 getItemCount(StaticFunctionTag* base, BSFixedString fileName, BSFixedString itemType);

	bool RegisterFuncs(VMClassRegistry* registry);
}
