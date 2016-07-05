#pragma once

#include "plugin_library\PluginLibrary.h"
#include "iostream"
#include "string"

typedef void(*TestCases);

std::string configProperties[9] = {
	"startDate",
	"lastWorkoutDate",
	"lastSyncDate",
	"firstWorkoutDate",
	"workoutCount"
	"weeksWorkedOut"
	"avgPointsPerWorkout"
	"totalPoints"
	"workoutsThisWeek"
};
void initialiseConfig() {
	for (std::string s : configProperties) {
		config.setConfigProperty(s, 0);
	}
}
//Tests for isOldSave()
TestCases tests = Test_iSOldSave_ConfigDoesntExist_False;
void Test_iSOldSave_ConfigDoesntExist_False();
void Test_isOldSave_ConfigIsInDefaultState_True();
void Test_isOldSave_ConfigIsCurrent_False();
void Test_isOldSave_LastConfigFailsToUpdate_False();
void Test_isOldSave_ConfigIsMoreRecent_True();
void Test_isOldSave_ConfigIsEmpty_Undefined();
void Test_isOldSave_ConfigIsMissingValuesButValidAndContainsSyncDate_Undefined();
void Test_isOldSave_ConfigIsMissingValuesButValidAndMissingSyncDate_Undefined();
void Test_isOldSave_ConfigIsCorruptedAndUnreadable_Undefined();
void Test_isOldSave_Config_Is_Empty();

