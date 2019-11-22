#include "JSONHandler.h"

#ifndef __RAWDATAHANDLER_H_INCLUDED__
#define __RAWDATAHANDLER_H_INCLUDED__

#ifdef COMPILE_MYLIBRARY   
#define MYLIBRARY_EXPORT __declspec(dllexport) 
#else   
#define MYLIBRARY_EXPORT __declspec(dllimport) 
#endif

class MYLIBRARY_EXPORT RawDataHandler : public JSONHandler
{
public:
	RawDataHandler();

	void refresh();

	void getDefaultData();

	void clear();

	int getWorkoutCount();

	json getWorkout(int workoutNumber);

	int getResponseCode();

	void setData(json newData);
};
#endif