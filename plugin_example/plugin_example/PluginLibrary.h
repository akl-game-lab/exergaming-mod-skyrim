#include "string"
#include "Windows.h"
#include "ShellApi.h"
#include "ctime"
#include "tchar.h"
#include "vector"
#include "json.hpp"
#include "ConfigHandler.h"
#include "DebugHandler.h"
#include "RawDataHandler.h"
#include "WeekHandler.h"

using json = nlohmann::json;

/**********************************************************************************************************
*	Globals
*/

#ifdef COMPILE_MYLIBRARY   
#define MYLIBRARY_EXPORT __declspec(dllexport) 
#else   
#define MYLIBRARY_EXPORT __declspec(dllimport) 
#endif

class MYLIBRARY_EXPORT PluginFunctions
{
public:
	int WEIGHT = 0;
	int HEALTH = 1;
	int STAMINA = 2;
	int MAGICKA = 3;
	char ITEM_SEPARATOR = ';';
	char FIELD_SEPARATOR = ',';
	float EXP_INCREASE_RATE = 0.1;
	// every x levels there should be improvement
	float LEVEL_IMPROVEMENT = 12.0;
	float ESTIMATED_LEVELS_PER_WEEK = 3.0;
	std::string WEB_SERVICE_DIR = "Data\\SKSE\\Plugins";

	/**********************************************************************************************************
	*	Handlers
	*/

	DebugHandler debug;
	ConfigHandler config;
	RawDataHandler rawData;
	WeekHandler weekHandler;

	/**********************************************************************************************************
	*	Helpers
	*/

	//Splits a string by the given delimeter
	std::vector<std::string> split(const std::string &s, char delim);

	//Converts a json representation of a workout to a formatted string
	std::string workoutToString(json workout);

	//Returns the week number that the workout date passed is in where week 1 is the week that the first workout was synced
	int getWeekForWorkout(__int64 firstTime, __int64 workoutTime);

	//Returns the day of the week that is the number of days from the start of the week, with the day of the configs startDate as the first day of the week.
	int getDayOfConfigWeek(__int64 date);

	//Returns a float representation of the number of levels gained from the workout passed to the method
	float getWeightForWorkout(json workout, int level);

	//Makes the service call to get raw data from the server
	void makeServiceCall(std::string type, std::string username, std::string fromDate, std::string toDate);

	//Updates the Weeks.xml file to contain all workouts logged to date
	std::string updateWeeks(int level);

	/**********************************************************************************************************
	*	Functions
	*/

	//Returns the current date
	__int64 currentDate();

	//Checks if the current save is old
	bool isOldSave(int creationDate);

	//Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
	std::string getWorkoutsFromBestWeek(__int64 creationDate);

	//Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	std::string getLevelUpsAsString(std::string outstandingLevel, std::string workoutsString);

	//Returns true if there is another level up and sets the health,stamina and magicka values
	bool isNthLevelUp(std::string levelUpsString, int n);

	//Returns the health, stamina or magicka component of the given level up
	int getLevelComponent(std::string levelUpsString, int n, std::string type);

	//Returns the outstanding level as a string
	std::string getOutstandingLevel(std::string levelUpsString);

	//Makes a service call to fetch workouts
	int startNormalFetch(std::string gameID, std::string username);

	//Starts the poll for new workouts when the user requests a check
	int startForceFetch(std::string gameID, std::string username);

	//Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
	std::string getWorkoutsString(int level);

	//Returns the number of workouts in the raw data file
	int getRawDataWorkoutCount();

	//Allows papyrus to clear the debug
	void clearDebug();

	//Checks if the given username is valid
	bool validUsername(std::string gameID, std::string username);

	//Returns a shortened username to fit in the menu screen
	std::string getShortenedUsername(std::string username);

	//Virtually presses the given key
	void pressKey(std::string key);

	//Gets the number of exercise.com points needed to level up
	int getPointsToNextLevel(float outstandingWeight);

	//Updates the config file to match the config object
	void updateConfig();

	//Returns a%b
	int mod(int a, int b);

	//Returns a/b
	int divide(int a, int b);
};