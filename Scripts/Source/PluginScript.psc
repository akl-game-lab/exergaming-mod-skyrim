scriptName PluginScript Hidden

	;Returns the current date to the calling papyrus script
	int function currentDate() global native

	;Checks if the current save is old
	bool function isOldSave(string creationDate) global native

	;Returns workouts logged between the given date to now as a string (format is "W,H,S,M;W,H,S,M...")
	string function fetchWorkouts(string gameID, string userName) global native

	;Returns the best week between the creation date of the calling save and now
	int function getBestWeek(string creationDate) global native

	;Returns the given weeks workouts as a string (format is "W,H,S,M;W,H,S,M...")
	string function getNthWeeksWorkouts(int weekNumber) global native

	;Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
	string function getLevelUpsAsString(string outstandingLevel, string workoutsString) global native

	;Returns true if there is a level up
	bool function isNthLevelUp(string levelUpsString, int n) global native

	;Returns the health, stamina or magicka component of the given level up
	int function getLevelComponent(string levelUpsString, int n, string type) global native