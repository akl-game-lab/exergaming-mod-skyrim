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
	
	//Stores strings that are retrieved from xml files
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
			outputFile << "<?xml version=\"1.0\"?><data></data>";
			outputFile.close();
		}
	}

	//Writes the given text to the debug log
	void writeToDebug(std::string line)
	{
		std::fstream debugFile;
		debugFile.open("Debug.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		debugFile << line << "\n";
		debugFile.close();
	}

	//Clears the debug log
	void clearDebug()
	{
		std::ofstream debugFile;
		debugFile.open("Debug.txt");
		debugFile << "";
		debugFile.close();
	}

	/**********************************************************************************************************
	*	Date Helpers
	*/

	//Returns the given date in the given format
	std::string toFormattedDate(time_t date, std::string format)
	{
		writeToDebug("Entered toFormattedDate(" + std::to_string(date) + "," + format +").");
		struct tm * timeinfo;
		timeinfo = localtime(&date);
		char buffer[80];
		strftime(buffer, 80, format.c_str(), timeinfo);
		writeToDebug("Exiting toFormattedDate().");
		return buffer;
	}

	//Returns the date that is the sunday of the week to which the given date belongs
	time_t getStartOfWeekFromDate(time_t date)
	{
		writeToDebug("Entered getStartOfWeekFromDate(" + std::to_string(date) + ").");
		std::string startOfWeek;
		time_t dayOfWeek = _atoi64(toFormattedDate(date, "%w").c_str());
		time_t untruncatedStartOfWeek = date - dayOfWeek*SECONDS_PER_DAY;
		struct tm * timeinfo;
		timeinfo = localtime(&untruncatedStartOfWeek);
		timeinfo->tm_sec = 0;
		timeinfo->tm_min = 0;
		timeinfo->tm_hour = 0;
		writeToDebug("Exiting getStartOfWeekFromDate().");
		return mktime(timeinfo);
	}

	//Returns the year component of the given date
	time_t getYear(time_t date)
	{
		writeToDebug("Entered getYear().");
		time_t year = _atoi64(toFormattedDate(date, "%Y").c_str());
		writeToDebug("Exiting getYear().");
		return year;
	}

	//Returns the week number that is the number of weeks since the start of the year that the week belongs to
	time_t getWeekNumber(time_t date)
	{
		writeToDebug("Entered getWeekNumber().");
		time_t weekNumber = _atoi64(toFormattedDate(date, "%U").c_str());
		writeToDebug("Exiting getWeekNumber().");
		return weekNumber;
	}

	/**********************************************************************************************************
	*	Handlers
	*/

	//Clears the raw data file
	void clearRawData()
	{
		std::ofstream debugFile;
		debugFile.open("Raw_Data.xml");
		debugFile << "";
		debugFile.close();
	}

	//Returns an Xpath query component that selects the element with the given name in the given position
	std::string toPathWithPos(std::string nodeName, int nodePosition)
	{
		writeToDebug("Entered toPathWithPos().");
		std::string pathWithPos = "//" + nodeName + "[position() = " + std::to_string(nodePosition + 1) + "]";
		writeToDebug("Exiting toPathWithPos().");
		return pathWithPos;
	}

	//Creates an xml node with the given name and text in the given xml doc
	MSXML::IXMLDOMNodePtr createAndFillNode(MSXML::IXMLDOMDocument2Ptr xmlDoc, std::string name, std::string text)
	{
		writeToDebug("Entered createAndFillNode().");
		MSXML::IXMLDOMNodePtr node = xmlDoc->createNode(MSXML::NODE_ELEMENT, toBSTR(name), _T(""));
		node->put_text(toBSTR(text));
		writeToDebug("Exiting createAndFillNode().");
		return node;
	}

	//Returns a reference to an instantiated xml doc for the given file
	MSXML::IXMLDOMDocument2Ptr getDoc(std::string fileName)
	{
		writeToDebug("Entered getDoc().");
		checkXmlFile(fileName);
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
		writeToDebug("Exiting getDoc().");
		return xmlDoc;
	}

	class WeekHandler{
	private:
		MSXML::IXMLDOMDocument2Ptr doc;

		MSXML::IXMLDOMNodePtr getWeek(int weekNumber)
		{
			writeToDebug("Entered WeekHandler->getWeek().");
			MSXML::IXMLDOMNodePtr week = doc->selectSingleNode(toBSTR(toPathWithPos("week", weekNumber)));
			writeToDebug("Exiting WeekHandler->getWeek().");
			return week;
		}

		MSXML::IXMLDOMNodePtr getWorkoutForWeek(int weekNumber, int workoutNumber)
		{
			writeToDebug("Entered WeekHandler->getWorkoutForWeek().");
			MSXML::IXMLDOMNodePtr workout = getWeek(weekNumber)->selectSingleNode(toBSTR(toPathWithPos("workout", workoutNumber)));
			writeToDebug("Exiting WeekHandler->getWorkoutForWeek().");
			return workout;
		}

		void save()
		{
			writeToDebug("Entered WeekHandler->save().");
			doc->save(_T("Weeks.xml"));
			writeToDebug("Exiting WeekHandler->save().");
		}

	public:
		WeekHandler()
		{
			doc = getDoc("Weeks.xml");
		}

		int getWeekCount()
		{
			writeToDebug("Entered WeekHandler->getWeekCount().");
			int count = doc->selectNodes("//week")->Getlength();
			writeToDebug("Exiting WeekHandler->getWeekCount().");
			return count;
		}

		int getWorkoutCountForWeek(int weekNumber)
		{
			writeToDebug("Entered WeekHandler->getWorkoutCountForWeek().");
			int count = doc->selectNodes(toBSTR(toPathWithPos("week",weekNumber) + "//workout"))->Getlength();
			writeToDebug("Exiting WeekHandler->getWorkoutCountForWeek().");
			return count;
		}

		time_t getWeekStart(int weekNumber)
		{
			writeToDebug("Entered WeekHandler->getWeekStart().");
			time_t weekStart = _atoi64(std::string(getWeek(weekNumber)->selectSingleNode("startDate")->Gettext()).c_str());
			writeToDebug("Exiting WeekHandler->getWeekStart().");
			return weekStart;
		}

		std::string getWorkoutProperty(int weekNumber, int workoutNumber, std::string propertyName)
		{
			writeToDebug("Entered WeekHandler->getWorkoutProperty().");
			std::string propertyValue = getWorkoutForWeek(weekNumber, workoutNumber)->selectSingleNode(toBSTR(propertyName))->Gettext();
			writeToDebug("Exiting WeekHandler->getWorkoutProperty().");
			return propertyValue;
		}

		void addWeek(time_t date)
		{
			writeToDebug("Entered WeekHandler->addWeek(" + std::to_string(date) + ").");
			MSXML::IXMLDOMNodePtr week = doc->createNode(MSXML::NODE_ELEMENT, _T("week"), _T(""));
			week->appendChild(createAndFillNode(doc, "startDate", std::to_string(getStartOfWeekFromDate(date))));
			doc->documentElement->appendChild(week);
			save();
			writeToDebug("Exiting WeekHandler->addWeek().");
		}

		void addWorkoutToWeek(int weekNumber, Workout workout)
		{
			writeToDebug("Entered WeekHandler->addWorkoutToWeek(" + std::to_string(weekNumber) + ").");
			MSXML::IXMLDOMNodePtr week = getWeek(weekNumber);
			MSXML::IXMLDOMNodePtr workoutNode = doc->createNode(MSXML::NODE_ELEMENT, _T("workout"), _T(""));
			workoutNode->appendChild(createAndFillNode(doc,"date",workout.date));
			workoutNode->appendChild(createAndFillNode(doc,"weight", workout.weight));
			workoutNode->appendChild(createAndFillNode(doc,"h", workout.health));
			workoutNode->appendChild(createAndFillNode(doc,"s", workout.stamina));
			workoutNode->appendChild(createAndFillNode(doc,"m", workout.magicka));
			week->appendChild(workoutNode);
			save();
			writeToDebug("Exiting WeekHandler->addWorkoutToWeek().");
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
			writeToDebug("Entered ConfigHandler->getConfigProperty().");
			std::string propertyValue = doc->selectSingleNode(toBSTR(propertyName))->Gettext();
			writeToDebug("Exiting ConfigHandler->getConfigProperty().");
			return propertyValue;
		}

		void setConfigProperty(std::string propertyName, std::string value)
		{
			writeToDebug("Entered ConfigHandler->setConfigProperty().");
			doc->selectSingleNode(toBSTR(propertyName))->Puttext(toBSTR(value));
			save();
			writeToDebug("Exiting ConfigHandler->setConfigProperty().");
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
			writeToDebug("Entered RawDataHandler->getWorkoutCount().");
			int count = doc->selectNodes("/data/workouts/*")->Getlength();
			writeToDebug("Exiting RawDataHandler->getWorkoutCount().");
			return count;
		}

		std::string getWorkoutProperty(int workoutNumber, std::string propertyName)
		{
			writeToDebug("Entered RawDataHandler->getWorkoutProperty().");
			MSXML::IXMLDOMNodePtr workout = doc->selectSingleNode(toBSTR("/data/workouts/workout[position() = " + std::to_string(workoutNumber + 1) + "]"));
			MSXML::IXMLDOMNodePtr propertyNode = workout->selectSingleNode(toBSTR(propertyName));
			std::string propertyValue = propertyNode->Gettext();
			writeToDebug("Exiting RawDataHandler->getWorkoutProperty().");
			return propertyValue;
		}
	};

	/**********************************************************************************************************
	*	Plugin Helpers
	*/

	float configure(Workout workout)
	{
		writeToDebug("Entered configure().");
		//Hamish to replace
		writeToDebug("Exiting configure().");
		return 1;
	}

	//If the week to which the given date belongs to exists, the week number is returned
	int weekExists(time_t date, WeekHandler weekHandler)
	{
		writeToDebug("Entered weekExists().");
		int weekCount = weekHandler.getWeekCount();
		if (weekCount != 0){
			for (int i = 0; i < weekHandler.getWeekCount(); i++)
			{
				time_t weekStart = weekHandler.getWeekStart(i);
				if ((getYear(weekStart) == getYear(date)) && (getWeekNumber(weekStart) == getWeekNumber(date))){
					writeToDebug("Exiting weekExists().");
					return i;
				}

			}
		}
		writeToDebug("Exiting weekExists().");
		return -1;
	}

	Workout getWorkout(int workoutNumber, RawDataHandler rawDataHandler)
	{
		writeToDebug("Entered getWorkout().");
		Workout workout;
		workout.date = std::to_string(_atoi64(rawDataHandler.getWorkoutProperty(workoutNumber, "workoutDate").c_str())/1000);
		workout.health = rawDataHandler.getWorkoutProperty(workoutNumber, "health");
		workout.stamina = rawDataHandler.getWorkoutProperty(workoutNumber, "stamina");
		workout.magicka = rawDataHandler.getWorkoutProperty(workoutNumber, "magicka");
		writeToDebug("Exiting getWorkout().");
		return workout;
	}

	//Updates the Weeks.xml file to contain all workouts logged to date
	std::string updateWeeks()
	{
		writeToDebug("Entered updateWeeks().");
		std::string workoutsAsString = "";
		int health = 0, stamina = 0, magicka = 0;
		RawDataHandler rawDataHandler;
		int workoutCount = rawDataHandler.getWorkoutCount();
		WeekHandler weekHandler;

		//for each workout
		for (int workoutNumber = 0; workoutNumber < workoutCount; workoutNumber++)
		{
			time_t date = _atoi64(rawDataHandler.getWorkoutProperty(workoutNumber, "workoutDate").c_str())/1000;
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
		writeToDebug("Exiting updateWeeks().");
		return workoutsAsString;
	}
}