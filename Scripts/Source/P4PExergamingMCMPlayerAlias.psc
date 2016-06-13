scriptname P4PExergamingMCMPlayerAlias extends referenceAlias  

import PluginScript

string property syncedUserName auto
int property creationDate auto
string property outstandingLevel auto
bool property forceFetchMade auto
bool property normalFetchMade auto
bool property saveRequested auto
int property pollStartTime auto
int pollDuration = 120
int workoutCount = 0;
int pollInterval = 1;
int pollCount = 0;

event OnPlayerLoadGame()
	creationDate = currentDate()
	clearDebug()
	forceFetchMade = false
	if (syncedUserName != "")
		showDebugMessage("Currently synced with " + syncedUserName)
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		workoutCount = getConfigProperty("workoutCount") as int
		startNormalFetch("Skyrim",syncedUserName)
		normalFetchMade = true
		;if(isOldSave(creationDate))
			;showDebugMessage("Old save detected.")
			;workouts = getWorkoutsFromBestWeek(weekNumber)
		;endIf
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif
	RegisterForUpdate(pollInterval)
endEvent

event onUpdate()
	if(saveRequested == true)
		saveRequested = false
		Game.requestSave()
	endIf
	if (normalFetchMade == true && pollCount % 5 == 0)
		if (workoutCount < getConfigProperty("workoutCount") as int)
			checkLevelUps()
			forceFetchMade = false
		elseIf (forceFetchMade == false)
			showDebugMessage("No workouts found.")
		endIf
		normalFetchMade = false
	endIf
	if (forceFetchMade == true && pollCount % 10 == 0)
		pollCount = 0;
		normalFetchMade = false
		if (currentDate() - pollStartTime < pollDuration)
			workoutCount = getConfigProperty("workoutCount") as int
			debug.Notification("Checking for recent workouts.")
			startNormalFetch("Skyrim",syncedUserName)
			normalFetchMade = true;
		else
			showDebugMessage("No recent workouts found.")
			forceFetchMade = false
		endIf
	endIf
	pollCount = pollCount + 1;
endEvent

function checkLevelUps()
	string workouts = getWorkoutsString(Game.getPlayer().getLevel());
	if(workouts == "Prior Workout")
		showDebugMessage("Prior workouts detected.")
		doLevelUp(4,3,3)
	else
		string levelUpsString = getLevelUpsAsString(outstandingLevel,workouts)
		;level ups start at index 1 as index 0 holds the outstanding level up
		int n = 1
		bool shouldContinue = isNthLevelUp(levelUpsString,n)
		while (shouldContinue)
			int health = getLevelComponent(levelUpsString,n,"H")
			int stamina = getLevelComponent(levelUpsString,n,"S")
			int magicka = getLevelComponent(levelUpsString,n,"M")
			doLevelUp(health,stamina,magicka)
			n = n + 1
			shouldContinue = isNthLevelUp(levelUpsString,n)
		endWhile
		outstandingLevel = getOutstandingLevel(levelUpsString)
		updateXpBar(levelUpsString)
	endIf
	saveRequested = true
endFunction

;Increment the player level and give the player a perk point
function doLevelUp(int health, int stamina, int magicka)
	Actor player = Game.getPlayer()
	int currentLevel = player.getLevel()
	player.modActorValue("health", health)
	player.modActorValue("stamina", stamina)
	player.modActorValue("magicka", magicka)
	Game.setPlayerLevel(currentLevel + 1)
	
	currentLevel = player.getLevel()
	Game.setPerkPoints(Game.getPerkPoints() + 1)
	showDebugMessage("Congratulations.\nYou have reached level " + currentLevel + "!")
	showDebugMessage("Health (+" + health + ")\nStamina (+" + stamina + ")\nMagicka (+" + magicka + ")")
endFunction

function showDebugMessage(string msg)
	debug.messageBox(msg)
	Utility.wait(0.1)
endFunction

;update the xp bar to show the progress gained
function updateXpBar(string levelUpsString)
	float outstandingHealth = getLevelComponent(levelUpsString,0,"H")
	float outstandingStamina = getLevelComponent(levelUpsString,0,"S")
	float outstandingMagicka = getLevelComponent(levelUpsString,0,"M")
	float outstandingWeight = outstandingHealth + outstandingStamina + outstandingMagicka
	Game.setPlayerExperience(outstandingWeight)
endFunction