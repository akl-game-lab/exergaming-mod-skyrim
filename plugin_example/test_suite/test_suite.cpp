// test_suite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "plugin_example\Helpers.h"

using namespace plugin;

void testGetDoc()
{
	printf("getDoc ");
	MSXML::IXMLDOMDocument2Ptr doc = plugin::getDoc("Weeks.xml");
	if (doc != NULL)
	{
		printf("succeeded.\n");
		return;
	}
	printf("failed.\n");
}

void testGetWeekCount()
{
	printf("getWeekCount ");
	WeekHandler wH;
	int weeks = wH.getWeekCount();
	printf("returned %d\n",weeks);
}

void testGetWorkoutCount()
{
	printf("getWorkoutCount ");
	RawDataHandler rDH;
	int workouts = rDH.getWorkoutCount();
	printf("returned %d\n", workouts);
}

void testGetWorkoutCountForWeek()
{
	printf("getWorkoutCountForWeek ");
	WeekHandler wH;
	int workouts = wH.getWorkoutCountForWeek(0);
	printf("returned %d\n", workouts);
}

void testRawDataGetWorkoutProperty()
{
	printf("rawDataGetWorkoutProperty ");
	RawDataHandler rDH;
	std::cout << "returned " << rDH.getWorkoutProperty(0,"Date") << "\n";
}

void testWeekExists()
{
	/*printf("weekExists ");
	WeekHandler wH;
	int result = weekExists(1000000000000, wH);
	if ( result == -1)
	{
		printf("succeeded.\n");
		return;
	}
	printf("failed. Expected -1 Got ");
	std::cout << result << "\n";*/
}

void testDateFormatting()
{
	/*
	printf("toUnformattedDate ");
	time_t result = toUnformattedDate("2016/04/25-11:29:01");
	std::cout << "returns " << result << ". Which formatted is " << toFormattedDate(result, FULL_DATE_FORMAT);*/
}

void testGetStartOfWeekFromDate()
{
	/*printf("getStartOfWeekFromDate ");
	int result = getStartOfWeekFromDate(100000000000);
	if (result == "2016/04/24")
	{
		printf("succeeded.");
		return;
	}
	printf("failed.");*/
}

void testAddWeek()
{
	/*printf("addWeek ");
	WeekHandler wH;
	int weekCountBefore = wH.getWeekCount();
	wH.addWeek("3000/12/02-00:00:00");
	int weekCountAfter = wH.getWeekCount();
	if (weekCountAfter == weekCountBefore + 1)
	{
		printf("succeeded.\n");
		return;
	}
	printf("failed.\n");*/
}

void testAddWorkoutToWeek()
{
	printf("addWorkoutToWeek ");
	WeekHandler wH;
	int workoutCountBefore = wH.getWorkoutCountForWeek(0);
	Workout w;
	w.date = "1";
	w.health = "1";
	w.stamina = "1";
	w.magicka = "1";
	w.weight = "1";
	wH.addWorkoutToWeek(0,w);
	int workoutCountAfter = wH.getWorkoutCountForWeek(0);
	if (workoutCountAfter == workoutCountBefore + 1)
	{
		printf("succeeded.\n");
		return;
	}
	printf("failed.\n");
}

void testUpdateWeeks()
{
	printf("updateWeeks ");
	std::cout << "returned " << updateWeeks() << "\n";
}

void testGetLevelUpsAsString()
{
	printf("getLevelUpsAsString ");
}

int _tmain(int argc, _TCHAR* argv[])
{
	clearDebug();
	//testGetDoc();
	//testGetWeekCount();
	//testGetWorkoutCount();
	testGetWorkoutCountForWeek();
	//testRawDataGetWorkoutProperty();
	//testWeekExists();
	//testDateFormatting();
	//testGetStartOfWeekFromDate();
	//testAddWeek();
	testAddWorkoutToWeek();
	//testUpdateWeeks();
	//testGetLevelUpsAsString();
	getchar();
	return 0;
}

