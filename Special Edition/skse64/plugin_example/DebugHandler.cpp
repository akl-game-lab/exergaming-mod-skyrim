#include "DebugHandler.h"

int treeDepth = 0;
std::string lineStart = "--";
std::fstream debugFile;

//Writes the given text to the debug log
void DebugHandler::write(std::string line)
{
	debugFile.open("Debug.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	debugFile << lineStart << "[ " << line << " ]\n";
	debugFile.close();
}

//Writes an entry statement
void DebugHandler::entry(){}
void DebugHandler::entryReport(const char* caller)
{
	treeDepth++;
	lineStart = "->";
	write(caller);
}

//Writes an exit statement
void DebugHandler::exit(){}
void DebugHandler::exitReport(const char* caller)
{
	lineStart = "<-";
	write(caller);
	treeDepth--;
}

//Clears the debug log
void DebugHandler::clear()
{
	std::ofstream debugFile;
	debugFile.open("Debug.txt");
	debugFile << "";
	debugFile.close();
}

#define entry() entryReport(__FUNCTION__)
#define exit() exitReport(__FUNCTION__)