scriptname P4PExergamingMCMPlayerAlias extends referenceAlias  

import PluginScript

string property syncedUserName auto
int property creationDate auto
string property outstandingLevel auto
bool property forceFetchMade auto

int pollDuration = 120
int property pollStartTime auto

event OnPlayerLoadGame()
	clearDebug()
	forceFetchMade = false
	if (syncedUserName != "")
		;when the player loads in, need to grab the previous exercisedata if there is a synced account
		showDebugMessage("Currently synced with " + syncedUserName)
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		string workouts = fetchWorkouts("Skyrim",syncedUserName,Game.getPlayer().getLevel())
		;if(isOldSave(creationDate))
			;showDebugMessage("Old save detected.")
			;workouts = getWorkoutsFromBestWeek(weekNumber)
		;endIf
		checkLevelUps(workouts)
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif
	RegisterForUpdate(10)
endEvent

function checkLevelUps(string workouts)
	int weekNumber
	if(workouts != "")
		if(workouts == "Prior Workout")
			showDebugMessage("Prior workouts detected.")
			doLevelUP(4,3,3)
		else
			string levelUpsString = getLevelUpsAsString(outstandingLevel,workouts)

			;level ups start at index 1 as index 0 holds the outstanding level up
			int n = 1
			int health = 0
			int stamina = 0
			int magicka = 0
			bool levelUp = isNthLevelUp(levelUpsString,n)
			while (levelUp)
				health = getLevelComponent(levelUpsString,n,"H")
				stamina = getLevelComponent(levelUpsString,n,"S")
				magicka = getLevelComponent(levelUpsString,n,"M")
				doLevelUp(health,stamina,magicka)
				n = n + 1
				levelUp = isNthLevelUp(levelUpsString,n)
			endWhile
			outstandingLevel = getOutstandingLevel(levelUpsString)
			updateXpBar(levelUpsString)
		endIf
	elseIf(forceFetchMade == false)
		showDebugMessage("No workouts found this time")
	endIf
	creationDate = currentDate()
	Game.requestSave()
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

event onUpdate()
	if (forceFetchMade == true)
		if (currentDate() - pollStartTime < pollDuration)
			debug.Notification("Checking for recent workouts.")
			int workoutCount = getConfigProperty("workoutCount") as int
			string workouts = fetchWorkouts("Skyrim",syncedUserName,Game.getPlayer().getLevel())
			if (workoutCount < getConfigProperty("workoutCount") as int)
				checkLevelUps(workouts)
				forceFetchMade = false
			endIf
		else
			showDebugMessage("No recent workouts found.")
			forceFetchMade = false
		endIf
	endIf
endEvent