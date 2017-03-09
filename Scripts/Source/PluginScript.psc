scriptName PluginScript Hidden

;Returns the current date to the calling papyrus script
int function currentDate() global native

;Checks if the current save is old
bool function isOldSave(int creationDate) global native

;Returns the workouts from the day of the week of the creation date to the end of the best week between the creation date of the calling save and now as a string (format is "W,H,S,M;W,H,S,M...")
string function getWorkoutsFromBestWeek(int creationDate) global native

;Returns a string representation of the levels gained in the given week (format is "H,S,M;H,S,M...")
string function getLevelUpsAsString(string outstandingLevel, string workoutsString) global native

;Returns true if there is a level up
bool function isNthLevelUp(string levelUpsString, int n) global native

;Returns the health, stamina or magicka component of the given level up
int function getLevelComponent(string levelUpsString, int n, string type) global native

;Returns the outstanding level as a string
string function getOutstandingLevel(string levelUpsString) global native

;Makes a service call to fetch workouts
int function startNormalFetch(string gameID, string username) global native

;Starts the poll for new workouts when the user requests a check
int function startForceFetch(string gameID, string username) global native

;Returns workouts from Raw_Data.xml as a string (format is "W,H,S,M;W,H,S,M...")
string function getWorkoutsString(int level) global native

;Returns the number of workouts in the raw data file
int function getRawDataWorkoutCount() global native

;Allows papyrus to read the config
string function getConfigProperty(string propertyName) global native 

;Allows papyrus to clear the debug
function clearDebug() global native

;Checks if the given username is valid
bool function validUsername(string gameID, string username) global native

;Returns a shortened username to fit in the menu screen
string function getShortenedUsername(string username) global native

;Opens the skills menu
function openSkillsMenu() global native

;Gets the number of exercise.com points needed to level up
int function getPointsToNextLevel(float outstandingWeight) global native

;Updates the config file to match the config object
function updateConfig() global native

;Returns a%b
int function mod(int a, int b) global native

;Returns a/b
int function divide(int a, int b) global native
