#include "JSONHandler.h"

class RawDataHandler : public JSONHandler
{
public:
	RawDataHandler();

	void refresh();

	void clear();

	int getWorkoutCount();

	json getWorkout(int workoutNumber);

	std::string getResponseCode();
};