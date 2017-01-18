// test_suite.cpp : Defines the entry point for the console application.
//
#include "stdio.h"
#include "test_suite.h"

int results[sizeof(tests) / 4] = {};
int testNumber = 0;
int main()
{
	for (TestCases t : tests)
	{
		(*t)();
		testNumber++;
		if (testNumber > 12)
		{
			//break;
		}
	}
	std::cout << "Total number of cases ran: " << sizeof(tests) / 4 << "\n";
	std::cout << "\n|";
	for (int r : results)
	{
		std::cout << (r == 0 ? "." : (r == 1 ? "+" : "-"));
	}
	std::cout << "|\n";
	getchar();
}

//#define assertToString(bool) assertToStringWithName(bool,__FUNCTION__)
//#define assertToStringWithInt(bool, int) assertToStringWithNameAndInt(bool,__FUNCTION__,int)

//Content of this function written blindly by Henry
std::string assertToString(bool expression) {
	if (!expression) {
		results[testNumber] = -1;
		return (std::string)": failed\n";
	}
	else {
		return (std::string)" ";
	}
}
/*Also guessed by Henry*/
std::string assertToStringWithInt(bool expression, int subCase) {
	std::string string = std::to_string(subCase);
	return (std::string)" ";
}


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
void Test_isOldSave_ConfigShouldUpdateIfOutOfDate_Unspecified() {
	//initialiseConfig();
	//pluginFunctions.config.setConfigProperty("lastSyncDate", 1467690000); //config is 1 minute before the creation date (it failed to update the last time. Indicates as error)
	//pluginFunctions.isOldSave(1467690060);
	//std::cout << assertToString(pluginFunctions.config.getConfigProperty("lastSyncDate") == 1467690060);
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
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.75,750,0,0;0.25,250,0,0").compare("0.000000,0.000000,0.000000;10,0,0") == 0); //I think the code is not taking into account the decimal points
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForStaminaOverFourWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.5,0,300,0;0.25,0,150,0;0.2,0,120,0;0.05,0,30,0").compare("0.000000,0.000000,0.000000;0,10,0") == 0);
}
void Test_getLevelUpAsString_NoOutstandingExactlyOneLevelForMagickaOverFiveWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.1,0,0,40;0.2,0,0,80;0.3,0,0,120;0.3,0,0,120;0.1,0,0,40").compare("0.000000,0.000000,0.000000;0,0,10") == 0);
}
void Test_getLevelUpAsString_NoOutstandingWithPartLevelForHealth_String() {
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.333333,50,0,0;0.333333,50,0,0").compare("0.666666,0.000000,0.000000") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.666666,500,0,0;0.333333,250,0,0").compare("0.999999,0.000000,0.000000") == 0, 2); //is it intentional?
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.666666,500,0,0;0.666666,500,0,0").compare("0.333332,0.000000,0.000000;10,0,0") == 0, 3);
}
void Test_getLevelUpAsString_NoOutstandingWithPartLevelForStamina_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.333333,0,200,0;0.333333,0,200,0").compare("0.000000,0.666666,0.000000") == 0);
}
void Test_getLevelUpAsString_NoOutstandingWithPartLevelForMagicka_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "0.666666,0,0,1000;0.666666,0,0,1000").compare("0.000000,0.000000,0.333332;0,0,10") == 0);
}
void Test_getLevelUpAsString_OutstandingLevelsExactlyOneLevelForHealthOverTwoWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.500000,0.000000,0.000000", "0.25,750,0,0;0.25,750,0,0").compare("0.000000,0.000000,0.000000;10,0,0") == 0);
}
void Test_getLevelUpAsString_OutstandingLevelsExactlyOneLevelForStaminaOverTwoWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.750000,0.000000", "0.125,0,300,0;0.125,0,300,0").compare("0.000000,0.000000,0.000000;0,10,0") == 0);
}
void Test_getLevelUpAsString_OutstandingLevelsExactlyOneLevelForMagickaOverTwoWorkouts_String() {
	std::cout << assertToString(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.250000", "0.25,0,0,40;0.5,0,0,80").compare("0.000000,0.000000,0.000000;0,0,10") == 0);
}
void Test_getLevelUpAsString_OutstandingLevelsInAllAttributesOverTwoWorkouts_String() {
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.500000,0.300000,0.100000", "0.25,750,0,0;0.35,750,0,0").compare("0.500000,0.000000,0.000000;6,3,1") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.200000,0.000000,0.000000", "0.25,750,0,0;0.35,250,100,0").compare("0.700000,0.100000,0.000000") == 0, 2);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.300000,0.200000", "1,100,0,0;0.2,10,5,5").compare("0.600000,0.050000,0.050000;5,3,2") == 0, 3);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.200000,0.500000,0.100000", "0.7,100,100,500;1,900,300,300").compare("0.300000,0.100000,0.100000;2,5,3;4,2,4") == 0, 4);
}
void Test_getLevelUpAsString_AllAttributesTestRounding() {
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "1,550,450,0").compare("0.000000,0.000000,0.000000;6,4,0") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "2,108,88,4").compare("0.000000,0.000000,0.000000;5,4,1;5,4,1") == 0, 2); //that is a little too high, but the alternative may get -ve numbers
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.275000,0.225000,0.000000", "1,550,450,0").compare("0.275000,0.225000,0.000000;6,4,0") == 0, 3);
	std::cout << assertToStringWithInt(pluginFunctions.getLevelUpsAsString("0.000000,0.000000,0.000000", "10.5,549,449,2").compare("0.274500,0.224500,0.001000;5,4,1;5,4,1;5,4,1;5,4,1;5,4,1;5,4,1;5,4,1;5,4,1;5,4,1;5,4,1") == 0, 4);
}

void Test_isNthLevelUp_BlankString_False() { //levelup string should never be empty
	std::cout << assertToString(pluginFunctions.isNthLevelUp("", 1) == false);
}
void Test_isNthLevelUp_NoLevelsNEqualOne_False() {
	std::cout << assertToString(pluginFunctions.isNthLevelUp("0.200000,0.200000,0.200000", 1) == false);
}
void Test_isNthLevelUp_OneLevelNEqualOne_True() {
	std::cout << assertToString(pluginFunctions.isNthLevelUp("0.000000,0.000000,0.000000;5,3,2", 1) == true);
}
void Test_isNthLevelUp_OneLevelNEqualTwo_False() {
	std::cout << assertToString(pluginFunctions.isNthLevelUp("0.000000,0.000000,0.000000;5,3,2", 2) == false);
}
void Test_isNthLevelUp_TwoLevelNEqualOne_True() {
	std::cout << assertToString(pluginFunctions.isNthLevelUp("0.000000,0.000000,0.000000;5,3,2;0,0,10", 1) == true);
}
void Test_isNthLevelUp_TwoLevelNEqualTwo_True() {
	std::cout << assertToString(pluginFunctions.isNthLevelUp("0.000000,0.000000,0.000000;5,3,2;0,0,10", 2) == true);
}
void Test_isNthLevelUp_TwoLevelNEqualThree_False() {
	std::cout << assertToString(pluginFunctions.isNthLevelUp("0.000000,0.000000,0.000000;5,3,2;0,0,10", 3) == false);
}

void Test_getLevelComponent_GetHealth1_10() {
	std::cout << assertToString(pluginFunctions.getLevelComponent("0.000000,0.100000,0.200000;10,0,0;5,2,3", 1, "H") == 10);
}
void Test_getLevelComponent_GetHealth2_5() {
	std::cout << assertToString(pluginFunctions.getLevelComponent("0.000000,0.100000,0.200000;10,0,0;5,2,3", 2, "H") == 5);
}
void Test_getLevelComponent_GetStamina1_0() {
	std::cout << assertToString(pluginFunctions.getLevelComponent("0.000000,0.100000,0.200000;10,0,0;5,2,3", 1, "S") == 0);
}
void Test_getLevelComponent_GetStamina2_2() {
	std::cout << assertToString(pluginFunctions.getLevelComponent("0.000000,0.100000,0.200000;10,0,0;5,2,3", 2, "S") == 2);
}
void Test_getLevelComponent_GetMagicka1_0() {
	std::cout << assertToString(pluginFunctions.getLevelComponent("0.000000,0.100000,0.200000;10,0,0;5,2,3", 1, "M") == 0);
}
void Test_getLevelComponent_GetMagicka2_3() {
	std::cout << assertToString(pluginFunctions.getLevelComponent("0.000000,0.100000,0.200000;10,0,0;5,2,3", 2, "M") == 3);
}

void Test_getOutstandingLevel_noLevelUp_String() {
	std::cout << assertToString(pluginFunctions.getOutstandingLevel("0.000000,0.100000,0.200000").compare("0.000000,0.100000,0.200000") == 0);
}
void Test_getOutstandingLevel_oneLevelUp_String() {
	std::cout << assertToString(pluginFunctions.getOutstandingLevel("0.000000,0.100000,0.200000;3,4,3").compare("0.000000,0.100000,0.200000") == 0);
}
void Test_getOutstandingLevel_multipleLevelUps_String() {
	std::cout << assertToStringWithInt(pluginFunctions.getOutstandingLevel("0.333333,0.100000,0.200000;3,4,3;7,1,2;0,0,10").compare("0.333333,0.100000,0.200000") == 0, 1);
	std::cout << assertToStringWithInt(pluginFunctions.getOutstandingLevel("0.500000,0.233333,0.200000;10,0,0;0,10,0;5,5,0;6,3,1").compare("0.500000,0.233333,0.200000") == 0, 2);
}

void Test_startNormalFetch_ValidRawData() {
	initialiseConfig();
	pluginFunctions.rawData.clear();
	std::cout << assertToStringWithInt(pluginFunctions.getRawDataWorkoutCount() == 0, 1);
	pluginFunctions.startNormalFetch("Skyrim", "paul@paulralph.name");
	std::cout << assertToStringWithInt(pluginFunctions.getRawDataWorkoutCount() > 0, 2);
}
void Test_startForceFetch_ValidRawData() {
	initialiseConfig();
	pluginFunctions.rawData.clear();
	std::cout << assertToStringWithInt(pluginFunctions.getRawDataWorkoutCount() == 0, 1);
	pluginFunctions.startForceFetch("Skyrim", "paul@paulralph.name");
	std::cout << assertToStringWithInt(pluginFunctions.getRawDataWorkoutCount() > 0, 2);
}

void Test_getShortenedUsername_PaulRalph_unchanged() {
	std::cout << assertToString(pluginFunctions.getShortenedUsername("PaulRalph").compare("PaulRalph") == 0);
}
void Test_getShortenedUsername_DrPaulRalph_unchanged() {
	std::cout << assertToString(pluginFunctions.getShortenedUsername("DrPaulRalph").compare("DrPaulRalph") == 0);
}
void Test_getShortenedUsername_PaulRalph123_shortened() {
	std::cout << assertToString(pluginFunctions.getShortenedUsername("PaulRalph123").compare("PaulRalp...") == 0);
}
void Test_getShortenedUsername_ReallyReallyLongString_shortened() {
	std::cout << assertToString(pluginFunctions.getShortenedUsername("ReallyReallyLongString").compare("ReallyRe...") == 0);
}
void Test_getShortenedUsername_PaulRalph0_unchanged() {
	std::cout << assertToString(pluginFunctions.getShortenedUsername("PaulRalph0").compare("PaulRalph0") == 0);
}
void Test_getShortenedUsername_P_unchanged() {
	std::cout << assertToString(pluginFunctions.getShortenedUsername("P").compare("P") == 0);
}

//How does it work if the user is on first week??
void Test_getPointsToNextLevel_FreshConfig_Minus1() {
	initialiseConfig();
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(0.0f) == -1);
}
void Test_getPointsToNextLevel_AfterOneWorkOut_Minus1() {
	initialiseConfig();
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(0.3f) == -1);
}
void Test_getPointsToNextLevel_AfterExactlyOneWorkOut_Minus1() {
	initialiseConfig();
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(0.3f) == -1);
}
void Test_getPointsToNextLevel_EarlyInSecondWeek_3500() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("avgPointsPerWorkout", 3000);
	pluginFunctions.config.setConfigProperty("weeksWorkedOut", 2);
	pluginFunctions.config.setConfigProperty("workoutsThisWeek", 1); //should never be 0 since it is checked after the workouts processed
	pluginFunctions.config.setConfigProperty("workoutCount", 6);
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(30) == 3500);
	std::cout << pluginFunctions.getPointsToNextLevel(0.3f) << "\n";
}
void Test_getPointsToNextLevel_LateInSecondWeek_1712() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("avgPointsPerWorkout", 1467);
	pluginFunctions.config.setConfigProperty("weeksWorkedOut", 2);
	pluginFunctions.config.setConfigProperty("workoutsThisWeek", 4);
	pluginFunctions.config.setConfigProperty("workoutCount", 9);
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(30) == 1711);
}
void Test_getPointsToNextLevel_Week5NoOutstandingLevel_5174() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("avgPointsPerWorkout", 2141);
	pluginFunctions.config.setConfigProperty("weeksWorkedOut", 5);
	pluginFunctions.config.setConfigProperty("workoutsThisWeek", 4);
	pluginFunctions.config.setConfigProperty("workoutCount", 33);
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(0.0f) == 5174);
}
void Test_getPointsToNextLevel_Week5AlmostLevel_1() {
	initialiseConfig();
	pluginFunctions.config.setConfigProperty("avgPointsPerWorkout", 127662);
	pluginFunctions.config.setConfigProperty("weeksWorkedOut", 5);
	pluginFunctions.config.setConfigProperty("workoutsThisWeek", 4);
	pluginFunctions.config.setConfigProperty("workoutCount", 51);
	std::cout << assertToString(pluginFunctions.getPointsToNextLevel(99.999999) == 0);
}
