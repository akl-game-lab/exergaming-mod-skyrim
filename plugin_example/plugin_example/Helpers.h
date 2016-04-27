#include "Windows.h"
#include <ShellApi.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <tchar.h>
#import <msxml6.dll> rename_namespace(_T("MSXML"))

namespace plugin
{
	/**********************************************************************************************************
	*	Globals
	*/

	int WEIGHT = 0;
	int HEALTH = 1;
	int STAMINA = 2;
	int MAGICKA = 3;
	int SECONDS_PER_DAY = 86400;
	char ITEM_SEPARATOR = ';';
	char FIELD_SEPARATOR = ',';
	std::string WEB_SERVICE_DIR = "Data\\webserviceTest\\Release";
	
	struct Workout
	{
		std::string date;
		std::string weight;
		std::string health;
		std::string stamina;
		std::string magicka;
	};

	/**********************************************************************************************************
	*	String Helpers
	*/

	//Converts a UTF8 string to a wide Unicode String
	std::wstring toWString(const std::string &str)
	{
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	//Converts a string to a BSTR
	BSTR toBSTR(std::string s)
	{
		return SysAllocStringLen(toWString(s).data(), toWString(s).size());
	}

	//Splits a string by the given delimeter
	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	/**********************************************************************************************************
	*	File Helpers
	*/

	//Checks if the given file exists
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

	//If the given xml file does not exist, this function initialises it
	void checkXmlFile(std::string fileName)
	{
		if (!fileExists(fileName))
		{
			std::ofstream outputFile(fileName);
			outputFile << "<?xml version=\"1.0\"?><Data></Data>";
			outputFile.close();
		}
	}

	/**********************************************************************************************************
	*	Date Helpers
	*/

	//Returns the given date in the given format
	std::string toFormattedDate(time_t date, std::string format)
	{
		struct tm * timeinfo;
		timeinfo = localtime(&date);
		char buffer[80];
		strftime(buffer, 80, format.c_str(), timeinfo);

		return buffer;
	}

	//Returns the date that is the sunday of the week to which the given date belongs
	int getStartOfWeekFromDate(int date)
	{
		std::string startOfWeek;
		int dayOfWeek = std::stoi(toFormattedDate(date, "%w"));
		time_t untruncatedStartOfWeek = date - dayOfWeek*SECONDS_PER_DAY;
		struct tm * timeinfo;
		timeinfo = localtime(&untruncatedStartOfWeek);
		timeinfo->tm_sec = 0;
		timeinfo->tm_min = 0;
		timeinfo->tm_hour = 0;
		return mktime(timeinfo);
	}

	//Returns the year component of the given date
	int getYear(int date)
	{
		return std::stoi(toFormattedDate(date, "%Y"));
	}

	//Returns the week number that is the number of weeks since the start of the year that the week belongs to
	int getWeekNumber(int date)
	{
		return std::stoi(toFormattedDate(date, "%U"));
	}

	/**********************************************************************************************************
	*	Handlers
	*/

	void writeToDebug(std::string line)
	{
		std::fstream debugFile;
		debugFile.open("Debug.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		debugFile << line << "\n";
		debugFile.close();
	}

	void clearDebug()
	{
		std::ofstream debugFile;
		debugFile.open("Debug.txt");
		debugFile << "";
		debugFile.close();
	}

	void clearRawData()
	{
		std::ofstream debugFile;
		debugFile.open("Raw_Data.xml");
		debugFile << "";
		debugFile.close();
	}

	std::string toPathWithPos(std::string nodeName, int nodePosition)
	{
		return std::string("//" + nodeName + "[position() = " + std::to_string(nodePosition + 1) + "]");
	}

	//Creates an xml node with the given name and text in the given xml doc
	MSXML::IXMLDOMNodePtr createAndFillNode(MSXML::IXMLDOMDocument2Ptr xmlDoc, std::string name, std::string text)
	{
		MSXML::IXMLDOMNodePtr node = xmlDoc->createNode(MSXML::NODE_ELEMENT, toBSTR(name), _T(""));
		node->put_text(toBSTR(text));
		return node;
	}

	MSXML::IXMLDOMDocument2Ptr getDoc(std::string fileName)
	{
		MSXML::IXMLDOMDocument2Ptr xmlDoc;
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			try{
				hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
				if (xmlDoc->load(toBSTR(fileName)) == VARIANT_TRUE)
				{
					xmlDoc->setProperty("SelectionLanguage", "XPath");
				}
			}
			catch (_com_error &e)
			{
				return NULL;
			}
		}
		return xmlDoc;
	}

	class WeekHandler{
	private:
		MSXML::IXMLDOMDocument2Ptr doc;

		MSXML::IXMLDOMNodePtr getWeek(int weekNumber)
		{
			return doc->selectSingleNode(toBSTR(toPathWithPos("week", weekNumber)));
		}

		MSXML::IXMLDOMNodePtr getWorkoutForWeek(int weekNumber, int workoutNumber)
		{
			return getWeek(weekNumber)->selectSingleNode(toBSTR(toPathWithPos("workout", workoutNumber)));
		}

		void save()
		{
			doc->save(_T("Weeks.xml"));
		}

	public:
		WeekHandler()
		{
			doc = getDoc("Weeks.xml");
		}

		int getWeekCount()
		{
			return doc->selectNodes("//week")->Getlength();
		}

		int getWorkoutCountForWeek(int weekNumber)
		{
			return doc->selectNodes(toBSTR(toPathWithPos("week",weekNumber) + "//workout"))->Getlength();
		}

		int getWeekStart(int weekNumber)
		{
			return std::stoi(std::string(getWeek(weekNumber)->selectSingleNode("startDate")->Gettext()));
		}

		std::string getWorkoutProperty(int weekNumber, int workoutNumber, std::string propertyName)
		{
			return getWorkoutForWeek(weekNumber, workoutNumber)->selectSingleNode(toBSTR(propertyName))->Gettext();
		}

		void addWeek(int date)
		{
			MSXML::IXMLDOMNodePtr week = doc->createNode(MSXML::NODE_ELEMENT, _T("Week"), _T(""));
			week->appendChild(createAndFillNode(doc, "startDate", std::to_string(getStartOfWeekFromDate(date))));
			doc->documentElement->appendChild(week);
			save();
		}

		void addWorkoutToWeek(int weekNumber, Workout workout)
		{
			MSXML::IXMLDOMNodePtr week = getWeek(weekNumber);
			MSXML::IXMLDOMNodePtr workoutNode = doc->createNode(MSXML::NODE_ELEMENT, _T("workout"), _T(""));
			workoutNode->appendChild(createAndFillNode(doc,"date",workout.date));
			workoutNode->appendChild(createAndFillNode(doc,"weight", workout.weight));
			workoutNode->appendChild(createAndFillNode(doc,"h", workout.health));
			workoutNode->appendChild(createAndFillNode(doc,"s", workout.stamina));
			workoutNode->appendChild(createAndFillNode(doc,"m", workout.magicka));
			week->appendChild(workoutNode);
			save();
		}
	};

	class ConfigHandler {
	private:
		MSXML::IXMLDOMDocument2Ptr doc;

		void save()
		{
			doc->save(_T("Config.xml"));
		}

	public:
		ConfigHandler()
		{
			doc = getDoc("Config.xml");
		}

		std::string getConfigProperty(std::string propertyName)
		{
			return doc->selectSingleNode(toBSTR(propertyName))->Gettext();
		}

		void setConfigProperty(std::string propertyName, std::string value)
		{
			doc->selectSingleNode(toBSTR(propertyName))->Puttext(toBSTR(value));
			save();
		}
	};

	class RawDataHandler {
	private:
		MSXML::IXMLDOMDocument2Ptr doc;

	public:
		RawDataHandler()
		{
			doc = getDoc("Raw_Data.xml");
		}

		int getWorkoutCount()
		{
			return doc->selectNodes("/data/workouts/*")->Getlength();
		}

		std::string getWorkoutProperty(int workoutNumber, std::string propertyName)
		{
			MSXML::IXMLDOMNodePtr workout = doc->selectSingleNode(toBSTR("/data/workouts/workout[position() = " + std::to_string(workoutNumber + 1) + "]"));
			MSXML::IXMLDOMNodePtr propertyNode = workout->selectSingleNode(toBSTR(propertyName));
			return propertyNode->Gettext();
		}
	};

	/**********************************************************************************************************
	*	Plugin Helpers
	*/

	float configure(Workout workout)
	{
		//Hamish to replace
		return 1;
	}

	//If the week to which the given date belongs to exists, the week number is returned
	int weekExists(int date, WeekHandler weekHandler)
	{
		int weekCount = weekHandler.getWeekCount();
		if (weekCount != 0){
			for (int i = 0; i < weekHandler.getWeekCount(); i++)
			{
				int weekStart = weekHandler.getWeekStart(i);
				if ((getYear(weekStart) == getYear(date)) && (getWeekNumber(weekStart) == getWeekNumber(date))){
					return i;
				}

			}
		}
		return -1;
	}

	Workout getWorkout(int workoutNumber, RawDataHandler rawDataHandler)
	{
		Workout workout;
		workout.date = std::to_string(std::stoi(rawDataHandler.getWorkoutProperty(workoutNumber, "workoutDate"))/1000);
		workout.health = rawDataHandler.getWorkoutProperty(workoutNumber, "health");
		workout.stamina = rawDataHandler.getWorkoutProperty(workoutNumber, "stamina");
		workout.magicka = rawDataHandler.getWorkoutProperty(workoutNumber, "magicka");
		return workout;
	}

	//Updates the Weeks.xml file to contain all workouts logged to date
	std::string updateWeeks()
	{
		std::string workoutsAsString = "";
		int health = 0, stamina = 0, magicka = 0;

		RawDataHandler rawDataHandler;
		int workoutCount = rawDataHandler.getWorkoutCount();
		writeToDebug(std::to_string(workoutCount) + " workout(s) found.");
		WeekHandler weekHandler;

		//for each workout
		for (int workoutNumber = 0; workoutNumber < workoutCount; workoutNumber++)
		{
			writeToDebug("Getting workout date.");
			writeToDebug(rawDataHandler.getWorkoutProperty(workoutNumber, "workoutDate"));
			int date = std::stoi(rawDataHandler.getWorkoutProperty(workoutNumber, "workoutDate"));
			//date = date/100;
			writeToDebug("Workout found for " + std::to_string(date));
			//check if the current workout belongs to an existing week
			int weekNumber = weekExists(date, weekHandler);
			if (weekNumber == -1)
			{
				weekHandler.addWeek(date);
				weekNumber = weekHandler.getWeekCount() - 1;
			}

			Workout workout = getWorkout(workoutNumber, rawDataHandler);

			//adjust the config to account for the new workout and gets the workouts weight
			workout.weight = std::to_string(configure(workout));

			weekHandler.addWorkoutToWeek(weekNumber, workout);

			if (workoutNumber > 0)
			{
				workoutsAsString = workoutsAsString + ";";
			}

			workoutsAsString = workoutsAsString + workout.weight + "," + workout.health + "," + workout.stamina + "," + workout.magicka;
		}
		return workoutsAsString;
	}
}