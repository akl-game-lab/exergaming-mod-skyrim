#include "MyPlugin.h"

#include "Windows.h"
#include <ShellApi.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <tchar.h>
#import <msxml6.dll> rename_namespace(_T("MSXML"))

class VMClassRegistry;

//this is a clean one
namespace MyPluginNamespace {

	int HEALTH = 1;
	int STAMINA = 2;
	int MAGICKA = 3;

	std::string webServiceDir = "Data\\webserviceTest\\Release";

	/*======================
		Helper functions
	======================*/

	// Convert an UTF8 string to a wide Unicode String
	std::wstring toWString(const std::string &str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	BSTR toBSTR(std::string s)
	{
		return SysAllocStringLen(toWString(s).data(), toWString(s).size());
	}

	//gets the points for the specified type from the given workout string
	int getWorkoutPoints(std::string workoutString, int type)
	{
		//get start point and range for the points string
		int range = 0, startPos = 0, endPos = 0;
		std::string startChar, endChar;
		if (type == MAGICKA)
		{
			startChar = "m";
			endPos = workoutString.length();
		}
		else{
			if (type == HEALTH)
			{
				startChar = "h";
				endChar = "s";

			}
			else if (type == STAMINA)
			{
				startChar = "s";
				endChar = "m";
			}
			endPos = workoutString.find(endChar);
		}

		startPos = workoutString.find(startChar) + 1;

		range = endPos - startPos;


		//get the points string
		std::string workoutPointsString = workoutString.substr(startPos, range);

		//return the points as an int
		return atoi(workoutPointsString.c_str());
	}

	bool fileExists(const std::string& name)
	{
		if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}

	MSXML::IXMLDOMNodePtr createAndFillNode(MSXML::IXMLDOMDocument2Ptr xmlDoc, std::string name, std::string text)
	{
		MSXML::IXMLDOMNodePtr node = xmlDoc->createNode(MSXML::NODE_ELEMENT, toBSTR(name), _T(""));
		node->put_text(toBSTR(text));
		return node;
	}

	std::string getFormattedDate(){
		time_t t = time(0);
		struct tm *lt = localtime(&t);
		std::stringstream currentDate;
		currentDate << std::setfill('0');
		currentDate << std::setw(4) << lt->tm_year + 1900 << "-"
			<< std::setw(2) << lt->tm_mon + 1 << "-"
			<< std::setw(2) << lt->tm_mday
			<< std::setw(2) << lt->tm_hour << ":"
			<< std::setw(2) << lt->tm_min << ":"
			<< std::setw(2) << lt->tm_sec;
		return currentDate.str();
	}

	void checkXmlFile(std::string fileName)
	{
		if (!fileExists(fileName))
		{
			std::ofstream outputFile(fileName);
			outputFile << "<?xml version=\"1.0\"?><Data></Data>";
			outputFile.close();
		}
	}

	int storeBatch(std::string startDate, std::string endDate)
	{
		int health = 0, stamina = 0, magicka = 0;

		int workoutCount = 0;

		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			try
			{
				MSXML::IXMLDOMDocument2Ptr xmlDoc;
				hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
				xmlDoc->setProperty("SelectionLanguage", "XPath");

				if (xmlDoc->load(_T("Raw_Data.xml")) != VARIANT_TRUE)
				{
					return -1;
				}
				else
				{
					MSXML::IXMLDOMNodeListPtr workouts = xmlDoc->selectNodes("/fiss/Data/*");

					workoutCount = workouts->Getlength();

					//for each workout
					for (int i = 0; i < workoutCount; i++)
					{
						//add the workout points
						std::string workoutString = workouts->Getitem(i)->Gettext();
						health += getWorkoutPoints(workoutString, HEALTH);
						stamina += getWorkoutPoints(workoutString, STAMINA);
						magicka += getWorkoutPoints(workoutString, MAGICKA);
					}
				}

				checkXmlFile("Batches.xml");

				if (xmlDoc->load(_T("Batches.xml")) != VARIANT_TRUE)
				{
					return 0;
				}
				else
				{
					MSXML::IXMLDOMNodePtr batchesNode = xmlDoc->createNode(MSXML::NODE_ELEMENT, _T("Batches"), _T(""));
					
					//create new xml batch
					MSXML::IXMLDOMNodePtr batchNode = xmlDoc->createNode(MSXML::NODE_ELEMENT, _T("Batch"), _T(""));
					xmlDoc->documentElement->appendChild(batchesNode);
					batchesNode->appendChild(batchNode);
					batchNode->appendChild(createAndFillNode(xmlDoc, "Start_Date", startDate));
					batchNode->appendChild(createAndFillNode(xmlDoc, "End_Date", endDate));
					batchNode->appendChild(createAndFillNode(xmlDoc, "H", std::to_string(health)));
					batchNode->appendChild(createAndFillNode(xmlDoc, "S", std::to_string(stamina)));
					batchNode->appendChild(createAndFillNode(xmlDoc, "M", std::to_string(magicka)));
					batchNode->appendChild(createAndFillNode(xmlDoc, "Workout_Count", std::to_string(workoutCount)));

					hr = xmlDoc->save(_T("Batches.xml"));
				}
			}
			catch (_com_error &e)
			{
			}
			CoUninitialize();
		}

		return workoutCount;
	}


	/*======================
		Plugin Functions
	======================*/

	BSFixedString currentDate(StaticFunctionTag* base) {
		return getFormattedDate().c_str();
	}

	UInt32 fetchXml(StaticFunctionTag* base, BSFixedString gameID, BSFixedString userName, BSFixedString fromDate) {

		//Take the arguments and put them into a set of parameters for the executable
		std::string sGameID(gameID.data);
		std::string sUserName(userName.data);
		std::string sFromDate(fromDate.data);
		std::string toDate = getFormattedDate();

		std::string exeParams = sGameID + " " + sUserName + " " + sFromDate + " " + toDate;

		LPCSTR swExeParams = exeParams.c_str();

		//Set the executable path
		std::string exePath = webServiceDir + "\\webserviceTest.exe";
		LPCSTR swExePath = exePath.c_str();

		//Execute the code that fetches the xml and stores it in the skyrim folder.
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
		return storeBatch(sFromDate, toDate);
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

	//gets the specified string from the specified xml doc
	BSFixedString getXmlString(StaticFunctionTag* base, BSFixedString fileName, BSFixedString itemType, UInt32 index, BSFixedString field)
	{
		BSFixedString returnString;
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			try
			{
				checkXmlFile("Batches.xml");
				
				MSXML::IXMLDOMDocument2Ptr xmlDoc;
				hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
				xmlDoc->setProperty("SelectionLanguage", "XPath");

				if (xmlDoc->load(toBSTR(fileName.data)) != VARIANT_TRUE)
				{
					returnString.data = ("Failed to load " + std::string(fileName.data)).c_str();
				}
				else
				{
					std::string selectionString = "/ Data / " + std::string(itemType.data) + "/*";
					MSXML::IXMLDOMNodeListPtr items = xmlDoc->selectNodes(toBSTR("/Data/" + std::string(itemType.data) + "/*"));
					MSXML::IXMLDOMNodePtr item = items->Getitem(index);
					return item->selectSingleNode(toBSTR(field.data))->Gettext();
				}
			}
			catch (_com_error &e)
			{
				returnString.data = "Exception";
			}
			CoUninitialize();
		}
		else
		{
			returnString.data = "Failed to initialise.";
		}

		return returnString;
	}

	//gets the number of items of the specified type from the specified xml doc
	UInt32 getItemCount(StaticFunctionTag* base, BSFixedString fileName, BSFixedString itemType)
	{
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			try
			{
				checkXmlFile("Batches.xml");

				MSXML::IXMLDOMDocument2Ptr xmlDoc;
				hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
				xmlDoc->setProperty("SelectionLanguage", "XPath");

				if (xmlDoc->load(toBSTR(fileName.data)) != VARIANT_TRUE)
				{
					return -1;
				}
				else
				{
					MSXML::IXMLDOMNodeListPtr items = xmlDoc->selectNodes(toBSTR("/Data/" + std::string(itemType.data) + "/*"));
					return items->Getlength();
				}
			}
			catch (_com_error &e)
			{
				return -2;
			}
			CoUninitialize();
		}

		return -3;
	}

	bool RegisterFuncs(VMClassRegistry* registry) {

		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString, BSFixedString>("fetchXml", "MyPluginScript", MyPluginNamespace::fetchXml, registry));

		registry->RegisterFunction(
			new NativeFunction0 <StaticFunctionTag, BSFixedString>("currentDate", "MyPluginScript", MyPluginNamespace::currentDate, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, bool, BSFixedString, BSFixedString>("isFirstWeekCompleted", "MyPluginScript", MyPluginNamespace::isFirstWeekCompleted, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("dayOfTheWeek", "MyPluginScript", MyPluginNamespace::dayOfTheWeek, registry));

		registry->RegisterFunction(
			new NativeFunction4 <StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, UInt32, BSFixedString>("getXmlString", "MyPluginScript", MyPluginNamespace::getXmlString, registry));

		registry->RegisterFunction(
			new NativeFunction2 <StaticFunctionTag, UInt32, BSFixedString, BSFixedString>("getItemCount", "MyPluginScript", MyPluginNamespace::getItemCount, registry));

		return true;
	}
}
