#include "JSONHandler.h"

#ifndef __WEEKHANDLER_H_INCLUDED__
#define __WEEKHANDLER_H_INCLUDED__
namespace
{
	int SECONDS_PER_DAY = 86400;
	int SECONDS_PER_WEEK = 604800;
}

class WeekHandler : public JSONHandler
{
public:
	WeekHandler();

	size_t getWorkoutHash(json workout);

	int getDayOfWeek(__int64 date);

	__int64 getStartOfDay(__int64 date);

	void addWeek(__int64 workoutDate);

	void addWorkoutToWeek(int weekNumber, json workout);

	//If the week to which the given date belongs to exists, the week number is returned
	void addWorkout(json workout);

	//Returns the workouts for the given week number
	json getWorkoutsFromWeek(int weekNumber);

	//Returns the startDate for the given week number
	__int64 getWeekStart(int weekNumber);

	//Returns the number of weeks currently stored
	int getWeekCount();
};
#endif
