// test_suite.cpp : Defines the entry point for the console application.
//

#include "test_suite.h"



int main()
{
	//WEB_SERVICE_DIR = "C:\\Program\ Files\ (x86)\\Steam\\steamapps\\common\\Skyrim\\Data\\SKSE\\Plugins";
	/*
	std::cout << "Testing isOldSave()\n";
	if (pluginFunctions.isOldSave(pluginFunctions.currentDate()))
	{
		std::cout << "Failure\n";
	}
	else
	{
		std::cout << "Success\n";
	}
	
	std::cout << "Attempting to construct raw data.\n";

	json newRawData = {
		{ "startDate", 0 },
		{ "lastWorkoutDate", 0 },
		{ "lastSyncDate", 0 },
		{ "firstWorkoutDate", 0 },
		{ "workoutCount", 0 },
		{ "weeksWorkedOut", 0 },
		{ "avgPointsPerWorkout", 0 },
		{ "totalPoints", 0 },
		{ "workoutsThisWeek", 0 }
	};

	std::cout << "Attempting to set raw data.\n";

	rawData.setData(newRawData);

	std::cout << "Set raw data.\n";

	/*
	std::cout << rawData.getWorkoutCount();
	std::cout << "\n";
	std::cout << "Testing getWorkoutsString()\n|";
	std::cout << pluginFunctions.getWorkoutsString(12);
	std::cout << "|";*/
	for (TestCases t : tests) {
		(*t)();
	}
	getchar();
}

//Tests for isOldSave()
void Test_iSOldSave_ConfigDoesntExist_False() {
	//check if it is the right case
	//check how to delete config
	std::cout << __func__ << ": ";
	std::cout << "not performed\n";
}
void Test_isOldSave_ConfigIsInDefaultState_True() {
	initialiseConfig();
	std::cout << __func__ << ": ";
	std::cout << (pluginFunctions.isOldSave(1467690060) == true ? "passed" : "failed") << "\n";
}
void Test_isOldSave_ConfigIsCurrent_False() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 1467690060);
	std::cout << __func__ << ": ";
	std::cout << (pluginFunctions.isOldSave(1467690060) == false ? "passed" : "failed") << "\n";
}
void Test_isOldSave_LastConfigFailsToUpdate_False() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 1467690000); //config last sync date is 1 minute before the creation date in the skyrim save file
	std::cout << __func__ << ": ";
	std::cout << (pluginFunctions.isOldSave(1467690060) == false ? "passed" : "failed") << "\n";
}
void Test_isOldSave_ConfigIsOldSave_True() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	std::cout << __func__ << ": ";
	std::cout << (pluginFunctions.isOldSave(1467690060) == true ? "passed" : "failed") <<"\n";
}
void Test_isOldSave_ConfigIsEmpty_Undefined() {
	//check how to break the config file in tests
	std::cout << __func__ << ": ";
	std::cout << "not performed\n";
}
void Test_isOldSave_ConfigIsMissingValuesButValidAndContainsSyncDate_Undefined() {
	std::cout << __func__ << ": ";
	std::cout << "not performed\n";
}
void Test_isOldSave_ConfigIsMissingValuesButValidAndMissingSyncDate_Undefined() {
	std::cout << __func__ << ": ";
	std::cout << "not performed\n";
}
void Test_isOldSave_ConfigIsCorruptedAndUnreadable_Undefined() {
	std::cout << __func__ << ": ";
	std::cout << "not performed\n";
}
void Test_isOldSave_Config_Is_Empty() {
	std::cout << __func__;
	std::cout << ": not performed\n";
}
void Test_isOldSave_CheckIfIdempotentOnOldSave_UnchangedConfig() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	std::cout << __func__ << ": ";
	std::cout << (pluginFunctions.isOldSave(1467690060) == pluginFunctions.isOldSave(1467690060) ? "passed" : "failed") << "\n";
}
void Test_isOldSave_CheckIfIdempotentOnCurrentSave_UnchangedConfig() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 146769060); //config is 1 minute after the creation date
	std::cout << __func__ << ": ";
	std::cout << (pluginFunctions.isOldSave(1467690060) == pluginFunctions.isOldSave(1467690060) ? "passed" : "failed") << "\n";
}
void Test_isOldSave_CheckIfCreateConfigOnDoesntExist_DefaultConfig() {
	std::cout << __func__;
	std::cout << ": not performed\n";
}
void Test_isOldSave_CheckIfRepairedConfigOnCorruptConfig_DefaultConfig() {
	std::cout << __func__;
	std::cout << ": not performed\n";
}
void Test_isOldSave_ConfigShouldNotUpdateOnOldSave_UnchangedConfig() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	pluginFunctions.isOldSave(1467690060);
	std::cout << __func__ << ": ";
	std::cout << (config.getConfigProperty("lastSyncDate") == 1467690120 ? "passed" : "failed") << "\n";
}
void Test_isOldSave_ConfigShouldNotUpdateOnCurrentSave_UnchangedConfig() {
	initialiseConfig();
	config.setConfigProperty("lastSyncDate", 1467690060); //config is 1 minute after the creation date
	pluginFunctions.isOldSave(1467690060);
	std::cout << __func__ << ": ";
	std::cout << (config.getConfigProperty("lastSyncDate") == 1467690060 ? "passed" : "failed") << "\n";
}