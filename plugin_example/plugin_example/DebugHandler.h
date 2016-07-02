#include <fstream>
#include <sstream>
class DebugHandler {
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