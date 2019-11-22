#include "WeekHandler.h"
#include "ConfigHandler.h"
#include "functional"

WeekHandler::WeekHandler()
{
	filePath = "Weeks.json";
	if (!fileExists())
	{
		content = {
			{"weeks",json::array({})}
		};
		saveJSON();
	}
	else
	{
		content = getJSON();
	}
}

size_t WeekHandler::getWorkoutHash(json workout)
{
	std::string weight = std::to_string((int)workout["weight"]);
	std::string health = std::to_string((int)workout["health"]);
	std::string stamina = std::to_string((int)workout["stamina"]);
	std::string magicka = std::to_string((int)workout["magicka"]);
	std::string date = std::to_string((int)workout["workoutDate"]);
	std::string workoutString = weight + health + stamina + magicka + date;

	std::hash<std::string> str_hash;

	return str_hash(workoutString);
}

//Returns the day of week for a given date
int WeekHandler::getDayOfWeek(__int64 date)
{
	struct tm *timeval;
	time_t tDate = (time_t)date;
	timeval = localtime(&tDate);
	return timeval->tm_wday;
}

//Returns the date that is the start of the day for the given date
__int64 WeekHandler::getStartOfDay(__int64 date)
{
	return (date / SECONDS_PER_DAY)*SECONDS_PER_DAY;
}

//Adds a week to the weeks file using the date of the new workout
void WeekHandler::addWeek(__int64 workoutDate)
{
	ConfigHandler config;
	__int64 startDate = config.getConfigProperty("startDate");
	int daysBetween = abs(getDayOfWeek(startDate) - getDayOfWeek(workoutDate));
	__int64 weekStart = getStartOfDay(workoutDate) - (daysBetween*SECONDS_PER_DAY);
	int weekCount = 0;
	
	if (!content["weeks"].empty())
	{
		weekCount = content["weeks"].size();
	}
	
	content["weeks"][weekCount]["startDate"] = weekStart;
	saveJSON();
}

void WeekHandler::addWorkoutToWeek(int weekNumber, json workout)
{
	json workouts = getWorkoutsFromWeek(weekNumber);
	for (int workoutNumber = 0; workoutNumber < workouts.size(); workoutNumber++)
	{
		if (getWorkoutHash(workout) == getWorkoutHash(content["weeks"][weekNumber]["workouts"][workoutNumber]))
		{
			return;
		}
	}
	content["weeks"][weekNumber]["workouts"][content["weeks"][weekNumber]["workouts"].size()] = workout;
	saveJSON();
}

//If the week to which the given date belongs to exists, the week number is returned
void WeekHandler::addWorkout(json workout)
{
	int weekCount = 0;

	if (!content["weeks"].empty())
	{
		weekCount = content["weeks"].size();
	}

	__int64 workoutDate = workout["workoutDate"];

	if (weekCount != 0) {
		for (int weekNumber = 0; weekNumber < weekCount; weekNumber++)
		{
			__int64 weekStart = content["weeks"][weekNumber]["startDate"];
			__int64 workoutDate = workout["workoutDate"];
			if (weekStart <= workoutDate && workoutDate - weekStart < SECONDS_PER_WEEK)
			{
				addWorkoutToWeek(weekNumber, workout);
				return;
			}
		}
	}

	addWeek(workoutDate);
	addWorkoutToWeek(weekCount, workout);
}

//Returns the workouts for the given week number
json WeekHandler::getWorkoutsFromWeek(int weekNumber)
{
	return content["weeks"][weekNumber]["workouts"];
}

//Returns the startDate for the given week number
__int64 WeekHandler::getWeekStart(int weekNumber)
{
	return content["weeks"][weekNumber]["startDate"];
}

//Returns the number of weeks currently stored
int WeekHandler::getWeekCount()
{
	return (int)content["weeks"].size();
}

//a do nothing implementation to compile
void WeekHandler::getDefaultData()
{
	content = {};
	saveJSON();
}

int WeekHandler::getWorkoutCount(int weekNumber) {
	return (int)content["weeks"][weekNumber]["workouts"].size();
}