// test_suite.cpp : Defines the entry point for the console application.
//

#include "test_suite.h"
/*
int results[sizeof(tests) / 4] = {};
int testNumber = 0;
*/int main()
{
	pluginFunctions.startNormalFetch("Skyrim","paul@paulralph.name");
	std::string workouts = pluginFunctions.getWorkoutsString(3);
	std::string levels = pluginFunctions.getLevelUpsAsString("0,0,0,0",workouts);
	std::cout << levels + "\n\n";
	std::cout << pluginFunctions.getOutstandingLevel(levels);
	//std::cout << pluginFunctions.getOutstandingLevel(pluginFunctions.getWorkoutsString(12));
	/*
	for (TestCases t : tests) 
	{
		(*t)();
		testNumber++;
	}
	
	std::cout << "\n|";
	for (int r : results)
	{
		std::cout << (r == 0 ? "." : (r == 1 ? "+" : "-"));
	}
	std::cout << "|\n";
	
	pluginFunctions.validUsername("Skyrim","paul");
	*/
	getchar();
}
/*
std::string assertToString(bool expression) {}

std::string assertToStringWithName(bool expression, const char* funcName) {
	if (!expression) {
		results[testNumber] = -1;
		return funcName + (std::string)": failed\n";
	}
	results[testNumber] = 1;
	return "";
}

#define assertToString(bool) assertToStringWithName(bool,__FUNCTION__)

//Tests for isOldSave()

void Test_isOldSave_ConfigIsInDefaultState_False() {
	initialiseConfig();
	std::cout << assertToString(pluginFunctions.isOldSave(1467690060) == false);
}
void Test_isOldSave_ConfigIsCurrent_False() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690060);
	std::cout << assertToString(pluginFunctions.isOldSave(1467690060) == false);
}
void Test_isOldSave_LastConfigFailsToUpdate_False() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690000); //config last sync date is 1 minute before the creation date in the skyrim save file
	std::cout << assertToString(pluginFunctions.isOldSave(1467690060) == false);
}
void Test_isOldSave_ConfigIsOldSave_True() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	std::cout << assertToString(pluginFunctions.isOldSave(1467690060) == true);
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
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	std::cout << assertToString(pluginFunctions.isOldSave(1467690060) == pluginFunctions.isOldSave(1467690060));
}
void Test_isOldSave_CheckIfIdempotentOnCurrentSave_UnchangedConfig() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 146769060); //config is 1 minute after the creation date
	std::cout << assertToString(pluginFunctions.isOldSave(1467690060) == pluginFunctions.isOldSave(1467690060));
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
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	pluginFunctions.isOldSave(1467690060);
	std::cout << assertToString(pluginFunctions.config.getConfigProperty("lastSyncDate") == 1467690120);
}
void Test_isOldSave_ConfigShouldNotUpdateOnCurrentSave_UnchangedConfig() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690060); //config is 1 minute after the creation date
	pluginFunctions.isOldSave(1467690060);
	std::cout << assertToString(pluginFunctions.config.getConfigProperty("lastSyncDate") == 1467690060);
}
*/