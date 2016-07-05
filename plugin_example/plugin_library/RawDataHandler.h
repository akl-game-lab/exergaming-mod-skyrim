#include "JSONHandler.h"

#ifdef COMPILE_MYLIBRARY
#define MYLIBRARY_EXPORT __declspec(dllexport)
#else
#define MYLIBRARY_EXPORT __declspec(dllimport)
#endif

#ifndef __RAWDATAHANDLER_H_INCLUDED__
#define __RAWDATAHANDLER_H_INCLUDED__

class MYLIBRARY_EXPORT RawDataHandler : public JSONHandler
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