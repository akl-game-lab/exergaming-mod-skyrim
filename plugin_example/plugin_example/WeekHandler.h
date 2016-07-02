#include "JSONHandler.h"

namespace
{
	int SECONDS_PER_DAY = 86400;
	int SECONDS_PER_WEEK = 604800;
}

class WeekHandler : public JSONHandler
{
public:
	WeekHandler();

	void addWeek(time_t startDate, time_t workoutDate);

	void addWorkoutToWeek(int weekNumber, json workout);

	//If the week to which the given date belongs to exists, the week number is returned
	void addWorkout(time_t startDate, json workout);

	//Returns the workouts for the given week number
	json getWorkoutsFromWeek(int weekNumber);

	//Returns the startDate for the given week number
	time_t getWeekStart(int weekNumber);

	//Returns the number of weeks currently stored
	int getWeekCount();
};
