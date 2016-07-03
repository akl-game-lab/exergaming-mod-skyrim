#include "WeekHandler.h"
WeekHandler::WeekHandler()
{
	filePath = "Weeks.json";
	data = getJSON(filePath);
}

void WeekHandler::addWeek(time_t startDate, time_t workoutDate)
{
	time_t weekStart = startDate + ((int)((startDate - workoutDate) / SECONDS_PER_DAY) / 7)*SECONDS_PER_WEEK;
	data["weeks"][data["weeks"].size()]["startDate"] = weekStart;
	saveJSON(filePath, data);
}

void WeekHandler::addWorkoutToWeek(int weekNumber, json workout)
{
	data["weeks"][weekNumber]["workouts"][data["weeks"][weekNumber]["workouts"].size()] = workout;
	saveJSON(filePath, data);
}

//If the week to which the given date belongs to exists, the week number is returned
void WeekHandler::addWorkout(time_t startDate,json workout)
{
	int weekCount = data["weeks"].size();
	time_t workoutDate = workout["workoutDate"];

	if (weekCount != 0) {
		for (int weekNumber = 0; weekNumber < weekCount; weekNumber++)
		{
			time_t weekStart = data["weeks"][weekNumber]["startDate"];
			time_t workoutDate = workout["workoutDate"];
			if (weekStart <= workoutDate && workoutDate - weekStart < SECONDS_PER_WEEK)
			{
				addWorkoutToWeek(weekNumber, workout);
				return;
			}
		}
	}
	addWeek(startDate, workoutDate);
	weekCount++;
	addWorkoutToWeek(weekCount - 1, workout);
}

//Returns the workouts for the given week number
json WeekHandler::getWorkoutsFromWeek(int weekNumber)
{
	return data["weeks"][weekNumber]["workouts"];
}

//Returns the startDate for the given week number
time_t WeekHandler::getWeekStart(int weekNumber)
{
	return data["weeks"][weekNumber]["startDate"];
}

//Returns the number of weeks currently stored
int WeekHandler::getWeekCount()
{
	return (int)data["weeks"].size();
}