#include "JSONHandler.h"

#ifndef __RAWDATAHANDLER_H_INCLUDED__
#define __RAWDATAHANDLER_H_INCLUDED__

class RawDataHandler : public JSONHandler
{
public:
	RawDataHandler();

	void refresh();

	void getDefaultRawData();

	void clear();

	int getWorkoutCount();

	json getWorkout(int workoutNumber);

	std::string getResponseCode();

	void setData(json newData);
};
#endif