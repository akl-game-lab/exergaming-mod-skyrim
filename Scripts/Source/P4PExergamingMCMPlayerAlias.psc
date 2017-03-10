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
endFunction

;Executes when a save finishes loading up
event OnPlayerLoadGame()
	initialise()
	if (syncedUserName != "");Check to see if user is synced with an account
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		oldSaveLoaded = isOldSave(creationDate as int)
		startNormalFetchWithErrorHandling()
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif
	int out = divide(6, 2)
	Debug.Notification("6 / 2 = " + out)
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
	endIf

	if (normalFetchMade == true && mod(pollCount,6) == 0)
		pollCount = 1
		normalFetchMade = false
		if(oldSaveLoaded == true)
			getLevelUps(getWorkoutsFromBestWeek(creationDate))
		elseIf (0 < getRawDataWorkoutCount());force fetch returned data
			getLevelUps(getWorkoutsString(Game.getPlayer().getLevel()))
			Utility.wait(2.0)
			forceFetchMade = false
		elseIf (forceFetchMade == false)
			noWorkoutsFound.show()
		endIf
	endIf

	if (forceFetchMade == true);
		debug.Notification("Checking for recent workouts.")
		int elapsed = currentDate() - pollStartTime
		if(elapsed >= pollDuration)
			searchComplete.show()
			forceFetchMade = false
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
	elseIf( serverResponse == 400 )
		debug.messageBox("CONFIGURATION ERROR\n\nPlease contact exergaming customer support with the current date and time.")
	elseIf( serverResponse == 200)
		normalFetchMade = true
	else
		debug.messageBox("SERVER ERROR\n\nPlease try again in a few minutes.\n\nIf this error persists, please contact exergaming customer support with the current date and time.")
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
	;Special case when workouts are found when the mod is turned on
	elseIf(workouts == "Prior Workout")
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
		levelUpsString = getLevelUpsAsString(outstandingLevel,workouts)
		;Level ups start at index 1 as index 0 holds the outstanding level up
		int n = 1
		bool shouldContinue = isNthLevelUp(levelUpsString,n)
		while (shouldContinue)
			int health = getLevelComponent(levelUpsString,n,"H")
			int stamina = getLevelComponent(levelUpsString,n,"S")
			int magicka = getLevelComponent(levelUpsString,n,"M")
			doLevelUp(health,stamina,magicka)
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
	endIf

	updateXpBar(levelUpsString, levelsUp, healthUp, staminaUp, magickaUp)
	
	saveRequested = true
endFunction

;Increment the player level and give the player a perk point
function doLevelUp(int health, int stamina, int magicka)
	Actor player = Game.getPlayer()
	int currentLevel = player.getLevel()
	int carryCapacityUp = (stamina/2)
	player.modActorValue("health", health)
	player.modActorValue("stamina", stamina)
	player.modActorValue("magicka", magicka)
	player.ModActorValue("CarryWeight", carryCapacityUp) ;Added to increase carry weight proportionally with stamina as in the vanilla version
	Game.setPlayerLevel(currentLevel + 1)
	currentLevel = player.getLevel()
	Game.setPerkPoints(Game.getPerkPoints() + 1)
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