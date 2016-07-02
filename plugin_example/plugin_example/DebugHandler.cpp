#include "DebugHandler.h"

int treeDepth = 0;
std::string lineStart = "--";
std::fstream debugFile;

//Writes the given text to the debug log
void DebugHandler::write(std::string line)
{
	debugFile.open("Debug.txt", std::fstream::in | std::fstream::out | std::fstream::app);

	for (int i = 0; i < treeDepth - 1; i++)
	{
		debugFile << "\t";
	}

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

DebugHandler debug;

extern "C" void __declspec(naked) _cdecl _penter(void) {
	_asm {
		push eax
			push ebx
			push ecx
			push edx
			push ebp
			push edi
			push esi
	}

	debug.entry();

	_asm {
		pop esi
			pop edi
			pop ebp
			pop edx
			pop ecx
			pop ebx
			pop eax
			ret
	}
}

extern "C" void __declspec(naked) _cdecl _pexit(void) {
	_asm {
		push eax
			push ebx
			push ecx
			push edx
			push ebp
			push edi
			push esi
	}

	debug.exit();

	_asm {
		pop esi
			pop edi
			pop ebp
			pop edx
			pop ecx
			pop ebx
			pop eax
			ret
	}
}
