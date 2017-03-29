scriptname P4PExergamingMCMPlayerAlias extends referenceAlias  

import PluginScript

string property syncedUserName auto
int property creationDate auto
string property outstandingLevel auto
bool property forceFetchMade auto
bool property normalFetchMade auto
bool property saveRequested auto
int property pollStartTime auto
bool property oldSaveLoaded auto

message property noWorkoutsFound auto
message property searchComplete auto
message property priorWorkouts auto
message property levelUpMessage auto
message property levelUpDetails auto
message property levelProgressMsg auto

;Event log file
String eventLog = "SkyrimExergameMod_EventLog"

float pollInterval = 0.5
int pollCount = 1

;Resets variables used for leveling or polling
function initialise()
	clearDebug()
	pollCount = 1
	pollStartTime = 0
	forceFetchMade = false
	oldSaveLoaded = false
	RegisterForUpdate(pollInterval)
	Debug.OpenUserLog(eventLog)
	Debug.TraceUser(eventLog, "Mod Turned On", 0)
	Debug.CloseUserLog(eventLog)
endFunction

;Resets variables used by the mod when it is turned off
function uninitialise()
	syncedUserName = ""
	;reset the exp variables
	Game.SetPlayerExperience(0)
	Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	clearDebug()
	pollCount = 1
	pollStartTime = 0
	forceFetchMade = false
	oldSaveLoaded = false
	Debug.OpenUserLog(eventLog)
	Debug.TraceUser(eventLog, "Mod Turned Off", 0)
	Debug.CloseUserLog(eventLog)
endFunction

;Executes when a save finishes loading up
event OnPlayerLoadGame()
	initialise()
	if (syncedUserName != "");Check to see if user is synced with an account
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		oldSaveLoaded = isOldSave(creationDate as int)
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Save Game loaded. File is an old save: " +oldSaveLoaded, 0)
		Debug.CloseUserLog(eventLog)
		startNormalFetchWithErrorHandling()
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Save Game loaded. File is an old save: " +oldSaveLoaded, 0)
		Debug.CloseUserLog(eventLog)
	endif
	int result = 0
	result = division(6, 2)
	Debug.MessageBox("six divided by two = "+result)
endEvent

;Executes automatically every second, called by the game
event onUpdate()
	int pollDuration = 120
	
	if(saveRequested == true)
		creationDate = currentDate()
		saveRequested = false
		Utility.WaitMenuMode(1)
		Game.requestSave()
		updateConfig()
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Game Saved", 0)
		Debug.CloseUserLog(eventLog)
	endIf

	if (normalFetchMade == true && mod(pollCount,6) == 0)
		pollCount = 1
		normalFetchMade = false
		if(oldSaveLoaded == true)
			getLevelUps(getWorkoutsFromBestWeek(creationDate))
			Debug.OpenUserLog(eventLog)
			Debug.TraceUser(eventLog, "Retreiving work outs from best week as this is an old save", 0)
			Debug.CloseUserLog(eventLog)
		elseIf (0 < getRawDataWorkoutCount());force fetch returned data
			getLevelUps(getWorkoutsString(Game.getPlayer().getLevel()))
			Utility.wait(2.0)
			forceFetchMade = false
			Debug.OpenUserLog(eventLog)
			Debug.TraceUser(eventLog, "Force fetch returned data", 0)
			Debug.CloseUserLog(eventLog)
		elseIf (forceFetchMade == false)
			noWorkoutsFound.show()
			Debug.OpenUserLog(eventLog)
			Debug.TraceUser(eventLog, "No workouts found", 0)
			Debug.CloseUserLog(eventLog)
		endIf
	endIf

	if (forceFetchMade == true);
		debug.Notification("Checking for recent workouts.")
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Force fetch started", 0)
		Debug.CloseUserLog(eventLog)
		int elapsed = currentDate() - pollStartTime
		if(elapsed >= pollDuration)
			searchComplete.show()
			forceFetchMade = false
			Debug.OpenUserLog(eventLog)
			Debug.TraceUser(eventLog, "Fetch from exercise.com Complete, retrieving data from our server", 0)
			Debug.CloseUserLog(eventLog)
			startNormalFetchWithErrorHandling()
		endIf
	endIf
	pollCount = pollCount + 1
endEvent

;Starts a normal fetch and handles the response to provide the user with error feedback
function startNormalFetchWithErrorHandling()
	int serverResponse = startNormalFetch("Skyrim",syncedUserName)
	if( serverResponse == 404)
		debug.messageBox("INVALID STATE ERROR\n\nPlease contact exergaming customer support with the current date and time.")
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Server Returned" +serverResponse, 0)
		Debug.CloseUserLog(eventLog)
	elseIf( serverResponse == 400 )
		debug.messageBox("CONFIGURATION ERROR\n\nPlease contact exergaming customer support with the current date and time.")
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Server Returned" +serverResponse, 0)
		Debug.CloseUserLog(eventLog)
	elseIf( serverResponse == 200)
		normalFetchMade = true
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Server Returned" +serverResponse, 0)
		Debug.TraceUser(eventLog, "Success", 0)
		Debug.CloseUserLog(eventLog)
	else
		debug.messageBox("SERVER ERROR\n\nPlease try again in a few minutes.\n\nIf this error persists, please contact exergaming customer support with the current date and time.")
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Unforseen error, Server Returned" +serverResponse, 0)
		Debug.CloseUserLog(eventLog)
	endIf
endFunction

;Uses workout data in string format outstandingHealth,outstandingStamina,outstandingMagicka;Health,Stamina,Magicka;Health,Stamina,Magicka;...
;Outstanding points are those which did not contribute to a full level up and act as the xp towards the next level
;The Health,Stamina,Magicka;... are the points for a given workout
function getLevelUps(string workouts)
	string levelUpsString = "0,0,0"
	
	int levelsUp = 0
	int	healthUp = 0
	int	staminaUp = 0
	int	magickaUp = 0

	;Special case when workouts are logged after the mod is turned on, for a date that was before the mod was turned on
	if(workouts == "Workout Logged Prior")
		debug.messageBox("REST DAY?\n\nThe only new workouts we could find were from before the date you started using our mod.\n\nYou'll only get level ups for workouts done after the mod was turned on.\n\nIf you've only just logged one, you can check using the \"Check for recent workouts\" button in the Exergaming Menu.")
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "The only work outs found were logged after the mod was turned on for a date prior to the mod being turned on, No level up applied", 0)
		Debug.CloseUserLog(eventLog)
	;Special case when workouts are found when the mod is turned on
	elseIf(workouts == "Prior Workout")
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Workouts logged prior to mod being turned on, generic level up being applied", 0)
		Debug.CloseUserLog(eventLog)
		priorWorkouts.show()
		;--------------------------------------------------
		levelsUp = 1
		healthUp = 4
		staminaUp = 3
		magickaUp = 3
		;--------------------------------------------------
		doLevelUp(healthUp,staminaUp,magickaUp)
		levelUpsString = "0,0,0;4,3,3"
	;General case
	else
		Debug.OpenUserLog(eventLog)
		Debug.TraceUser(eventLog, "Calculating Level up", 0)
		levelUpsString = getLevelUpsAsString(outstandingLevel,workouts)
		;Level ups start at index 1 as index 0 holds the outstanding level up
		int n = 1
		bool shouldContinue = isNthLevelUp(levelUpsString,n)
		while (shouldContinue)
			int health = getLevelComponent(levelUpsString,n,"H")
			int stamina = getLevelComponent(levelUpsString,n,"S")
			int magicka = getLevelComponent(levelUpsString,n,"M")
			doLevelUp(health,stamina,magicka)
			Debug.TraceUser(eventLog, "Applying Level up", 0)
			Debug.TraceUser(eventLog, "Health: "+health, 0)
			Debug.TraceUser(eventLog, "Stamina: "+stamina, 0)
			Debug.TraceUser(eventLog, "Magicka: "+magicka, 0)
			;--------------------------------------------------
			levelsUp = levelsUp + 1
			healthUp = healthUp + health
			staminaUp = staminaUp + stamina
			magickaUp = magickaUp + magicka
			;--------------------------------------------------
			n = n + 1
			shouldContinue = isNthLevelUp(levelUpsString,n)
		endWhile
		outstandingLevel = getOutstandingLevel(levelUpsString)
		Debug.TraceUser(eventLog, "Outstanding Level: "+outstandingLevel, 0)
	endIf

	Debug.TraceUser(eventLog, "Updating XP bar, levelUpsString: "+levelUpsString, 0)
	Debug.TraceUser(eventLog, "Updating XP bar, healthup: "+healthUp, 0)
	Debug.TraceUser(eventLog, "Updating XP bar, staminaup: "+staminaUp, 0)
	Debug.TraceUser(eventLog, "Updating XP bar, magickaup: "+magickaUp, 0)
	Debug.CloseUserLog(eventLog)

	updateXpBar(levelUpsString, levelsUp, healthUp, staminaUp, magickaUp)
	saveRequested = true
endFunction

;Increment the player level and give the player a perk point
function doLevelUp(int health, int stamina, int magicka)
	Debug.OpenUserLog(eventLog)
	Actor player = Game.getPlayer()
	int currentLevel = player.getLevel()

	Debug.TraceUser(eventLog, "Doing Level up, Current Level (preUp): " +currentLevel, 0)

	int carryCapacityUp = (stamina/2)  	;The division function is not working. this seems to work okay as the numbers are whole and simple.

	player.modActorValue("health", health)
	player.modActorValue("stamina", stamina)
	player.modActorValue("magicka", magicka)
	player.ModActorValue("CarryWeight", carryCapacityUp) ;Added to increase carry weight proportionally with stamina as in the vanilla version
	Game.setPlayerLevel(currentLevel + 1)
	currentLevel = player.getLevel()
	Game.setPerkPoints(Game.getPerkPoints() + 1)

	Debug.TraceUser(eventLog, "LevelUp Done: New Current Level" +currentLevel, 0)
	Debug.TraceUser(eventLog, "LevelUp Done: Health Increase" +health, 0)
	Debug.TraceUser(eventLog, "LevelUp Done: Magicka Increase" +magicka, 0)
	Debug.TraceUser(eventLog, "LevelUp Done: Stamina Increase" +stamina, 0)
	Debug.TraceUser(eventLog, "LevelUp Done: Carry Capacity increase" +carryCapacityUp, 0)
	Debug.TraceUser(eventLog, "LevelUp Done: Perkpoints incresed by One, New value:" +Game.GetPerkPoints(), 0)
	debug.CloseUserLog(eventLog)
endFunction

;update the xp bar to show the progress gained
function updateXpBar(string levelUpsString, int levelsUp, int healthUp, int staminaUp, int magickaUp)
	int outstandingHealth = getLevelComponent(levelUpsString,0,"H")
	int outstandingStamina = getLevelComponent(levelUpsString,0,"S")
	int outstandingMagicka = getLevelComponent(levelUpsString,0,"M")
	float outstandingWeight = outstandingHealth + outstandingStamina + outstandingMagicka
	;display message for progress to next level
	;first progress, second amount of workout
	if(levelsUp > 0)
		levelUpMessage.show(levelsUp,Game.getPlayer().getLevel(),healthUp,staminaUp,magickaUp)
	endIf
	if(outstandingWeight > 0)
		levelProgressMsg.show(outstandingWeight, getPointsToNextLevel(outstandingWeight))
	endIf
	Game.setPlayerExperience(Game.getExperienceForLevel(Game.getPlayer().getLevel())*(outstandingWeight/100))
endFunction