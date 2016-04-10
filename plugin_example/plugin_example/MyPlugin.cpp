#include "MyPlugin.h"
#include <windows.h>
#include <ShellApi.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

//this is a clean one
namespace MyPluginNamespace {
	float fetchXml(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName, BSFixedString lastSyncDate) {

		//takes the skyrim input paramaters and converts them to std::strings so that they can be manipulated
		std::string sGameID(gameID.data);
		std::string sUserName(userName.data);
		std::string sLastSyncDate(lastSyncDate.data);
		std::string fullString = sGameID + " " + sUserName + " " + sLastSyncDate;


		std::string exePath = "C:\\Users\\Mitchell\\Desktop\\exeTest\\webServiceTest.exe";
		LPCSTR swExePath = exePath.c_str();


		std::string exeParams = fullString;
		LPCSTR swExeParams = exeParams.c_str();

		//executes the code that fetches the xml and saves it into the skyrim folder.
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = swExePath;
		ShExecInfo.lpParameters = swExeParams;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOWMINNOACTIVE;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

		//moves the xml from the skyrim folder into the fiss folder so that it is readable by fiss.
		MoveFileWithProgress("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim\\xmlTestfile.txt", "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim\\Data\\SKSE\\Plugins\\FISS\\NEWXMLDATA.txt", NULL, NULL, MOVEFILE_REPLACE_EXISTING);
		//sleep for half a second to make sure that the file has been moved before trying to read it. 
		//work around is to try do the same thing as the exe call and wait for the file to be moved before trying to read it.
		Sleep(500);

		DWORD d = 0;

		return GetExitCodeProcess(ShExecInfo.hProcess, &d);
	}

	BSFixedString currentDate(StaticFunctionTag* base) {

		time_t t = time(0);
		struct tm *lt = localtime(&t);
		std::stringstream currentDate;
		currentDate << std::setfill('0');
		currentDate << std::setw(4) << lt->tm_year + 1900
			<< std::setw(2) << lt->tm_mon + 1
			<< std::setw(2) << lt->tm_mday
			<< std::setw(2) << lt->tm_hour
			<< std::setw(2) << lt->tm_min
			<< std::setw(2) << lt->tm_sec;

		return (currentDate.str().c_str());
	}

	bool isFirstWeekCompleted(StaticFunctionTag* base, BSFixedString firstImportDate, BSFixedString workoutDate) {

		long long int firstDate = atoll((firstImportDate.data));
		long long int futureDate = atoll((workoutDate.data));
		long long int weekConstant = 7000000;
		if ((futureDate - firstDate) >= weekConstant) {
			return true;
		}
		return false;
	}

	UInt32 dayOfTheWeek(StaticFunctionTag* base, BSFixedString firstImportDate, BSFixedString workoutDate) {

		long long int firstDate = atoll((firstImportDate.data));
		long long int futureDate = atoll((workoutDate.data));
		long long int difference = futureDate - firstDate;
		UInt32 roundedFirstDate = firstDate / 1000000;
		UInt32 roundedFutureDate = futureDate / 1000000;
		UInt32 day = roundedFutureDate - roundedFirstDate;
		return day;
	}


	bool RegisterFuncs(VMClassRegistry* registry) {

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, float, BSFixedString, BSFixedString, BSFixedString>("fetchXml", "MyPluginScript", MyPluginNamespace::fetchXml, registry));

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, BSFixedString>("currentDate", "MyPluginScript", MyPluginNamespace::currentDate, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("isFirstWeekCompleted", "MyPluginScript", MyPluginNamespace::isFirstWeekCompleted, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("dayOfTheWeek", "MyPluginScript", MyPluginNamespace::dayOfTheWeek, registry));

		return true;
	}
}
