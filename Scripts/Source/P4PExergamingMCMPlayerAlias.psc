scriptname P4PExergamingMCMPlayerAlias extends referenceAlias  

import PluginScript

string property syncedUserName auto
int property creationDate auto
string property outstandingLevel auto

event OnPlayerLoadGame()
	;when the player loads in, need to grab the previous exercisedata if there is a synced account
	creationDate = currentDate();
	showMessage("Currently synced with " + syncedUserName)
	if (syncedUserName != "")
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		checkLevelUps()
		game.requestSave()
	Else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif

endEvent

function showMessage(string msg)
	debug.messageBox(msg)
	Utility.wait(0.1)
endFunction

function checkLevelUps()
	string workouts
	int weekNumber
	workouts = fetchWorkouts("Skyrim",syncedUserName)
	;if(isOldSave(creationDate))
		;weekNumber = getBestWeek(creationDate)
		;workouts = getNthWeeksWorkouts(weekNumber)
	;endIf
	if(workouts != "")
		showMessage(workouts)
		string levelUpsString = getLevelUpsAsString(outstandingLevel,workouts)
		int n = 0
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
	else
		showMessage("No workouts found this time!")
	endIf
endFunction

;Increment the player level and give the player a perk point
function doLevelUp(int health, int stamina, int magicka)
	Actor player = Game.GetPlayer()
	int currentLevel = player.getLevel()
	player.ModActorValue("health", health)
	player.ModActorValue("stamina", stamina)
	player.ModActorValue("magicka", magicka)
	Game.setPlayerLevel(currentLevel + 1)
	
	currentLevel = player.getLevel()
	Game.setPerkPoints(Game.getPerkPoints() + 1)
	showMessage("Congratulations.\nYou have reached level " + currentLevel + "!")
	showMessage("Health (+" + health + ")\nStamina (+" + stamina + ")\nMagicka (+" + magicka + ")")
endFunction

;update the xp bar to show the progress gained
function updateXpBar(int requiredXPToLevelUp, int outstandingPoints)
	float progress = (outstandingPoints/requiredXPToLevelUp)
	Game.setPlayerExperience(progress)
endFunction