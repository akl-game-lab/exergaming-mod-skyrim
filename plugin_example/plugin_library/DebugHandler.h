#include <fstream>
#include <sstream>

#ifdef COMPILE_MYLIBRARY
#define MYLIBRARY_EXPORT __declspec(dllexport)
#else
#define MYLIBRARY_EXPORT __declspec(dllimport)
#endif

#ifndef __DEBUGHANDLER_H_INCLUDED__
#define __DEBUGHANDLER_H_INCLUDED__
class MYLIBRARY_EXPORT DebugHandler {
private:
	int treeDepth;
	std::string lineStart;
	std::fstream debugFile;

public:
	//Writes the given text to the debug log
	void write(std::string line);

	//Writes an entry statement
	void entry();
	void entryReport(const char* caller);

	//Writes an exit statement
	void exit();
	void exitReport(const char* caller);

	//Clears the debug log
	void clear();
};
#endif