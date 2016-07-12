// test_suite.cpp : Defines the entry point for the console application.
//

#include "test_suite.h"

int results[sizeof(tests) / 4] = {};
int testNumber = 0;
int main()
{
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
	getchar();
}

std::string assertToString(bool expression) {}

std::string assertToStringWithInt(bool expression, int subCase) {}

/*
if it is the first test, init results to 1, if any failed, set to -1
*/
std::string assertToStringWithName(bool expression, const char* funcName) {
	if (results[testNumber] == 0) {
		results[testNumber] = 1;
	}
	if (!expression) {
		results[testNumber] = -1;
		return funcName + (std::string)": failed\n";
	}
	//results[testNumber] *= 1; no longer needed
	return "";
}
std::string assertToStringWithNameAndInt(bool expression, const char* funcName, int subCase) {
	std::string string = std::string(funcName, 0, 99) + std::to_string(subCase);
	return assertToStringWithName(expression, string.c_str());
}

#define assertToString(bool) assertToStringWithName(bool,__FUNCTION__)
#define assertToStringWithInt(bool, int) assertToStringWithNameAndInt(bool,__FUNCTION__,int)

//Tests for currentDate()

void Test_currentDate_BetweenBeforeAndAfterAndLessThanOneMinuteApart() {
	time_t before; time_t after; time_t functionOut;
	time(&before);
	functionOut = pluginFunctions.currentDate();
	time(&after);
	std::cout << assertToStringWithInt(functionOut >= before, 1);
	std::cout << assertToStringWithInt(functionOut <= after, 2);
	std::cout << assertToStringWithInt((functionOut - before) < 3, 3); //3 seconds tolerance, the before and after should be done within 1 second realistically
	std::cout << assertToStringWithInt((-(functionOut - after)) < 3, 4);
}

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
void Test_isOldSave_ConfigShouldNotUpdateOnOldSave_UnchangedConfig() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690120); //config is 1 minute after the creation date
	pluginFunctions.isOldSave(1467690060);
	std::cout << assertToString(pluginFunctions.config.getConfigProperty("lastSyncDate") == 1467690120);
}
void Test_isOldSave_ConfigShouldNotUpdateOnCurrentSave_UnchangedConfig() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690060); //config is up to date
	pluginFunctions.isOldSave(1467690060);
	std::cout << assertToString(pluginFunctions.config.getConfigProperty("lastSyncDate") == 1467690060);
}
void Test_isOldSave_ConfigShouldUpdateIfOutOfDate_ChangedConfig() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690000); //config is 1 minute before the creation date (it failed to update the last time. Indicates as error)
	pluginFunctions.isOldSave(1467690060);
	std::cout << assertToString(pluginFunctions.config.getConfigProperty("lastSyncDate") == 1467690060);
}


void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForHealth_String() {
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,50,0,0").compare("0.000000,0.000000,0.000000;10,0,0") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,50000,0,0").compare("0.000000,0.000000,0.000000;10,0,0") == 0, 2);
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForStamina_String() {
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,0,100,0").compare("0.000000,0.000000,0.000000;0,10,0") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,0,10000,0").compare("0.000000,0.000000,0.000000;0,10,0") == 0, 2);
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForMagicka_String() {
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,0,0,25").compare("0.000000,0.000000,0.000000;0,0,10") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,0,0,25000").compare("0.000000,0.000000,0.000000;0,0,10") == 0, 2);
}
void Test_getLevelUpAsString_NoOutstandingExactlyTwoLevelForHealth_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,750,0,0;1,750,0,0").compare("0.000000,0.000000,0.000000;10,0,0;10,0,0") == 0);
}
void Test_getLevelUpAsString_NoOutstandingExactlyThreeLevelForStamina_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,0,150,0;1,0,150,0;1,0,150,0").compare("0.000000,0.000000,0.000000;0,10,0;0,10,0;0,10,0") == 0);
}
void Test_getLevelUpAsString_NoOutstandingExactlyFourLevelForMagicka_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,0,0,30;1,0,0,30;1,0,0,30;1,0,0,30").compare("0.000000,0.000000,0.000000;0,0,10;0,0,10;0,0,10;0,0,10") == 0);
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForHealthOverTwoWorkouts_String() {
	std::cout << pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.75,750,0,0;0.25,250,0,0"); //this output is wrong. I expect 1 level since the weights add up to 1
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.75,750,0,0;0.25,250,0,0").compare("0.000000,0.000000,0.000000;10,0,0") == 0);
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForStaminaOverFourWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.5,0,300,0;0.25,0,150,0;0.2,0,120,0;0.05,0,30,0").compare("0.000000,0.000000,0.000000;0,10,0") == 0);
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForMagickaOverFiveWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.1,0,0,40;0.2,0,0,80;0.3,0,0,120;0.3,0,0,120;0.1,0,0,40").compare("0.000000,0.000000,0.000000;0,0,10") == 0);
}