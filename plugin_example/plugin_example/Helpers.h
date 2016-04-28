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
	int ENTRY = 0;
	int EXIT = 1;
	int WEIGHT = 0;
	int HEALTH = 1;
	int STAMINA = 2;
	int MAGICKA = 3;
	int SECONDS_PER_DAY = 86400;
	char ITEM_SEPARATOR = ';';
	char FIELD_SEPARATOR = ',';
	std::string WEB_SERVICE_DIR = "Data\\webserviceTest\\Release";
	
	/**********************************************************************************************************
	*	Debug
	*/

	class DebugHandler {
	private:
		int treeDepth = 0;
	public:
		//Writes the given text to the debug log
		void write(int type, std::string line)
		{
			std::fstream debugFile;
			debugFile.open("Debug.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			std::string lineStart = "<-";
			if (type == ENTRY)
			{
				treeDepth++;
				lineStart = "->";
			}

			for (int i = 0; i < treeDepth - 1; i++)
			{
				debugFile << "\t";
			}

			debugFile << lineStart << "[ " << line << " ]\n";
			debugFile.close();

			if (type == EXIT)
			{
				treeDepth--;
			}
		}

		//Clears the debug log
		void clear()
		{
			std::ofstream debugFile;
			debugFile.open("Debug.txt");
			debugFile << "";
			debugFile.close();
		}
	};
	
	DebugHandler debug;

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

	/**********************************************************************************************************
	*	Handlers
	*/

	//Returns a reference to an instantiated xml doc for the given file
	MSXML::IXMLDOMDocument2Ptr getDoc(std::string fileName)
	{
		debug.write(ENTRY, "getDoc()");
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
		debug.write(EXIT, "getDoc()");
		return xmlDoc;
	}

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
			debug.write(ENTRY, "ConfigHandler->getConfigProperty()");
			std::string propertyValue = doc->selectSingleNode(toBSTR(propertyName))->Gettext();
			debug.write(EXIT, "ConfigHandler->getConfigProperty()");
			return propertyValue;
		}

		void setConfigProperty(std::string propertyName, std::string value)
		{
			debug.write(ENTRY, "ConfigHandler->setConfigProperty()");
			doc->selectSingleNode(toBSTR(propertyName))->Puttext(toBSTR(value));
			save();
			debug.write(EXIT, "ConfigHandler->setConfigProperty()");
		}
	};

	ConfigHandler config;

	class RawDataHandler {
	private:
		MSXML::IXMLDOMDocument2Ptr doc;

	public:
		RawDataHandler()
		{
			doc = getDoc("Raw_Data.xml");
		}

		void refresh()
		{
			doc = getDoc("Raw_Data.xml");
		}

		void clear()
		{
			std::ofstream debugFile;
			debugFile.open("Raw_Data.xml");
			debugFile << "";
			debugFile.close();
		}

		int getWorkoutCount()
		{
			debug.write(ENTRY, "RawDataHandler->getWorkoutCount()");
			int count = doc->selectNodes("/data/workouts/*")->Getlength();
			debug.write(EXIT, "RawDataHandler->getWorkoutCount()");
			return count;
		}

		std::string getWorkoutProperty(int workoutNumber, std::string propertyName)
		{
			debug.write(ENTRY, "RawDataHandler->getWorkoutProperty()");
			MSXML::IXMLDOMNodePtr workout = doc->selectSingleNode(toBSTR("/data/workouts/workout[position() = " + std::to_string(workoutNumber + 1) + "]"));
			MSXML::IXMLDOMNodePtr propertyNode = workout->selectSingleNode(toBSTR(propertyName));
			std::string propertyValue = propertyNode->Gettext();
			debug.write(EXIT, "RawDataHandler->getWorkoutProperty()");
			return propertyValue;
		}
	};

	RawDataHandler rawData;

	//Returns an Xpath query component that selects the element with the given name in the given position
	std::string toPathWithPos(std::string nodeName, int nodePosition)
	{
		debug.write(ENTRY, "toPathWithPos()");
		std::string pathWithPos = "//" + nodeName + "[position() = " + std::to_string(nodePosition + 1) + "]";
		debug.write(EXIT, "toPathWithPos()");
		return pathWithPos;
	}

	//Creates an xml node with the given name and text in the given xml doc
	MSXML::IXMLDOMNodePtr createAndFillNode(MSXML::IXMLDOMDocument2Ptr xmlDoc, std::string name, std::string text)
	{
		debug.write(ENTRY, "createAndFillNode()");
		MSXML::IXMLDOMNodePtr node = xmlDoc->createNode(MSXML::NODE_ELEMENT, toBSTR(name), _T(""));
		node->put_text(toBSTR(text));
		debug.write(EXIT, "createAndFillNode()");
		return node;
	}

	class Workout{
	public:
		time_t date;
		float weight;
		int health;
		int stamina;
		int magicka;

		Workout(int workoutNumber)
		{
			date = _atoi64(rawData.getWorkoutProperty(workoutNumber, "workoutDate").c_str()) / 1000;
			health = std::stoi(rawData.getWorkoutProperty(workoutNumber, "health"));
			stamina = std::stoi(rawData.getWorkoutProperty(workoutNumber, "stamina"));
			magicka = std::stoi(rawData.getWorkoutProperty(workoutNumber, "magicka"));
		}

		Workout(MSXML::IXMLDOMNodePtr node)
		{
			date = _atoi64(node->selectSingleNode("/date")->Gettext()) / 1000;
			health = std::stoi(std::string(node->selectSingleNode("/h")->Gettext()));
			stamina = std::stoi(std::string(node->selectSingleNode("/s")->Gettext()));
			magicka = std::stoi(std::string(node->selectSingleNode("/m")->Gettext()));
		}

		std::string to_string()
		{
			return std::to_string(weight) + "," + std::to_string(health) + "," + std::to_string(stamina) + "," + std::to_string(magicka);
		}
	};

	class WeekHandler
	{
	private:
		MSXML::IXMLDOMDocument2Ptr doc;

		MSXML::IXMLDOMNodePtr getWeek(int weekNumber)
		{
			debug.write(ENTRY, "WeekHandler->getWeek()");
			MSXML::IXMLDOMNodePtr week = doc->selectSingleNode(toBSTR(toPathWithPos("week", weekNumber)));
			debug.write(EXIT, "WeekHandler->getWeek()");
			return week;
		}

		MSXML::IXMLDOMNodePtr getWorkoutForWeek(int weekNumber, int workoutNumber)
		{
			debug.write(ENTRY, "WeekHandler->getWorkoutForWeek()");
			MSXML::IXMLDOMNodePtr workout = getWeek(weekNumber)->selectSingleNode(toBSTR(toPathWithPos("workout", workoutNumber)));
			debug.write(EXIT, "WeekHandler->getWorkoutForWeek()");
			return workout;
		}

		void save()
		{
			debug.write(ENTRY, "WeekHandler->save()");
			doc->save(_T("Weeks.xml"));
			debug.write(EXIT, "WeekHandler->save()");
		}

		//Returns the given date in the given format
		std::string toFormattedDate(time_t date, std::string format)
		{
			debug.write(ENTRY, "toFormattedDate()");
			struct tm * timeinfo;
			timeinfo = localtime(&date);
			char buffer[80];
			strftime(buffer, 80, format.c_str(), timeinfo);
			debug.write(EXIT, "toFormattedDate()");
			return buffer;
		}

		time_t getDayOfWeek(time_t date)
		{
			debug.write(ENTRY, "getDayOfWeek()");
			time_t dayOfWeek = _atoi64(toFormattedDate(date, "%w").c_str());
			debug.write(EXIT, "getDayOfWeek()");
			return dayOfWeek;
		}

		//Returns the date that is the sunday of the week to which the given date belongs
		time_t getStartOfWeekFromDate(time_t date)
		{
			debug.write(ENTRY, "getStartOfWeekFromDate()");
			std::string startOfWeek;
			time_t dayOfWeek = getDayOfWeek(date);
			time_t untruncatedStartOfWeek = date - dayOfWeek*SECONDS_PER_DAY;
			struct tm * timeinfo;
			timeinfo = localtime(&untruncatedStartOfWeek);
			timeinfo->tm_sec = 0;
			timeinfo->tm_min = 0;
			timeinfo->tm_hour = 0;
			debug.write(EXIT, "getStartOfWeekFromDate()");
			return mktime(timeinfo);
		}

		//Returns the year component of the given date
		time_t getYear(time_t date)
		{
			debug.write(ENTRY, "getYear()");
			time_t year = _atoi64(toFormattedDate(date, "%Y").c_str());
			debug.write(EXIT, "getYear()");
			return year;
		}

		//Returns the week number that is the number of weeks since the start of the year that the week belongs to
		time_t getWeekNumber(time_t date)
		{
			debug.write(ENTRY, "getWeekNumber()");
			time_t weekNumber = _atoi64(toFormattedDate(date, "%U").c_str());
			debug.write(EXIT, "getWeekNumber()");
			return weekNumber;
		}

	public:
		WeekHandler()
		{
			doc = getDoc("Weeks.xml");
		}

		int getWeekCount()
		{
			debug.write(ENTRY, "WeekHandler->getWeekCount()");
			int count = doc->selectNodes("//week")->Getlength();
			debug.write(EXIT, "WeekHandler->getWeekCount()");
			return count;
		}

		int getWorkoutCountForWeek(int weekNumber)
		{
			debug.write(ENTRY, "WeekHandler->getWorkoutCountForWeek()");
			int count = doc->selectNodes(toBSTR(toPathWithPos("week", weekNumber) + "//workout"))->Getlength();
			debug.write(EXIT, "WeekHandler->getWorkoutCountForWeek()");
			return count;
		}

		time_t getWeekStart(int weekNumber)
		{
			debug.write(ENTRY, "WeekHandler->getWeekStart()");
			time_t weekStart = _atoi64(std::string(getWeek(weekNumber)->selectSingleNode("startDate")->Gettext()).c_str());
			debug.write(EXIT, "WeekHandler->getWeekStart()");
			return weekStart;
		}

		std::string getWorkoutProperty(int weekNumber, int workoutNumber, std::string propertyName)
		{
			debug.write(ENTRY, "WeekHandler->getWorkoutProperty()");
			std::string propertyValue = getWorkoutForWeek(weekNumber, workoutNumber)->selectSingleNode(toBSTR(propertyName))->Gettext();
			debug.write(EXIT, "WeekHandler->getWorkoutProperty()");
			return propertyValue;
		}

		void addWeek(time_t date)
		{
			debug.write(ENTRY, "WeekHandler->addWeek()");
			MSXML::IXMLDOMNodePtr week = doc->createNode(MSXML::NODE_ELEMENT, _T("week"), _T(""));
			week->appendChild(createAndFillNode(doc, "startDate", std::to_string(getStartOfWeekFromDate(date))));
			doc->documentElement->appendChild(week);
			save();
			debug.write(EXIT, "WeekHandler->addWeek()");
		}

		void addWorkoutToWeek(int weekNumber, Workout workout)
		{
			debug.write(ENTRY, "WeekHandler->addWorkoutToWeek(" + std::to_string(weekNumber) + ")");
			MSXML::IXMLDOMNodePtr week = getWeek(weekNumber);
			MSXML::IXMLDOMNodePtr workoutNode = doc->createNode(MSXML::NODE_ELEMENT, _T("workout"), _T(""));
			workoutNode->appendChild(createAndFillNode(doc, "date", std::to_string(workout.date)));
			workoutNode->appendChild(createAndFillNode(doc, "weight", std::to_string(workout.weight)));
			workoutNode->appendChild(createAndFillNode(doc, "h", std::to_string(workout.health)));
			workoutNode->appendChild(createAndFillNode(doc, "s", std::to_string(workout.stamina)));
			workoutNode->appendChild(createAndFillNode(doc, "m", std::to_string(workout.magicka)));
			week->appendChild(workoutNode);
			save();
			debug.write(EXIT, "WeekHandler->addWorkoutToWeek()");
		}

		//If the week to which the given date belongs to exists, the week number is returned
		void addWorkout(Workout workout)
		{
			debug.write(ENTRY, "addWorkout()");
			int weekCount = getWeekCount();
			time_t workoutDate = workout.date;

			if (weekCount != 0){
				for (int weekNumber = 0; weekNumber < getWeekCount(); weekNumber++)
				{
					time_t weekStartDate = getWeekStart(weekNumber);
					if ((getYear(weekStartDate) == getYear(workoutDate)) && (getWeekNumber(weekStartDate) == getWeekNumber(workoutDate))){
						addWorkoutToWeek(weekNumber, workout);
						debug.write(EXIT, "addWorkout()");
						return;
					}

				}
			}

			addWeek(workoutDate);
			weekCount++;
			addWorkoutToWeek(weekCount - 1, workout);
			debug.write(EXIT, "addWorkout()");
		}

		std::string getWorkoutsFromBestWeek(time_t creationDate)
		{
			int weekCount = getWeekCount();
			float bestWeeksWeight = 0;
			std::string bestWeeksWorkouts = "";

			if (weekCount != 0){
				for (int weekNumber = 0; weekNumber < getWeekCount(); weekNumber++)
				{
					time_t weekStartDate = getWeekStart(weekNumber);

					//if the week in frame is after the creation date or is the week the save was made
					if ((getYear(weekStartDate) >= getYear(creationDate)) && (getWeekNumber(weekStartDate) >= getWeekNumber(creationDate))){
						float thisWeeksWeight;
						std::string thisWeeksWorkouts = "";
						MSXML::IXMLDOMNodeListPtr workouts = getWeek(weekNumber)->selectNodes("/workout");

						//for each workout
						for (int workoutNumber = 0; workoutNumber < workouts->Getlength(); workoutNumber++)
						{
							Workout workout(workouts->Getitem(workoutNumber));
							//if it was done on a day of the week after that of the creation date add the weight to the total weight for this week
							if (getDayOfWeek(workout.date) >= getDayOfWeek(creationDate)){
								thisWeeksWeight += workout.weight;
								if (workoutNumber > 0)
								{
									thisWeeksWorkouts = thisWeeksWorkouts + ";";
								}
								thisWeeksWorkouts = thisWeeksWorkouts + workout.to_string();
							}
						}

						//if this week is better update the best week
						if (thisWeeksWeight > bestWeeksWeight)
						{
							bestWeeksWeight = thisWeeksWeight;
							bestWeeksWorkouts = thisWeeksWorkouts;
						}
					}
				}
			}

			//return the selected best weeks workouts as a string from the day of the week the save was made (format is W,H,S,M;W,H,S,M...).

			return bestWeeksWorkouts;
		}
	};

	WeekHandler weekHandler;

	/**********************************************************************************************************
	*	Plugin Helpers
	*/

	float configure(Workout workout)
	{
		debug.write(ENTRY, "configure()");
		//Hamish to replace
		debug.write(EXIT, "configure()");
		return 1;
	}

	//Updates the Weeks.xml file to contain all workouts logged to date
	std::string updateWeeks()
	{
		debug.write(ENTRY, "updateWeeks()");
		rawData.refresh();
		std::string workoutsAsString = "";
		int health = 0, stamina = 0, magicka = 0;
		int workoutCount = rawData.getWorkoutCount();

		//for each workout
		for (int workoutNumber = 0; workoutNumber < workoutCount; workoutNumber++)
		{
			time_t date = _atoi64(rawData.getWorkoutProperty(workoutNumber, "workoutDate").c_str())/1000;

			Workout workout(workoutNumber);

			//adjust the config to account for the new workout and gets the workouts weight
			workout.weight = configure(workout);

			weekHandler.addWorkout(workout);

			if (workoutNumber > 0)
			{
				workoutsAsString = workoutsAsString + ";";
			}

			workoutsAsString = workoutsAsString + workout.to_string();
		}
		debug.write(EXIT, "updateWeeks()");
		return workoutsAsString;
	}
}