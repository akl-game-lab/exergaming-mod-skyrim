scriptname P4PExergamingMCMPlayerAlias extends referenceAlias  

import PluginScript

string property syncedUserName auto
int property creationDate auto
string property outstandingLevel auto

event OnPlayerLoadGame()
	if (syncedUserName != "")
		;when the player loads in, need to grab the previous exercisedata if there is a synced account
		showMessage("Currently synced with " + syncedUserName)
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		checkLevelUps()
		creationDate = currentDate()
		game.requestSave()
	Else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif
endEvent

function checkLevelUps()
	string workouts
	int weekNumber
	workouts = fetchWorkouts("Skyrim",syncedUserName,Game.getPlayer().getLevel())
	;if(isOldSave(creationDate))
		;showMessage("Old save detected.");
		;workouts = getWorkoutsFromBestWeek(weekNumber)
	;endIf
	if(workouts != "")
		if(workouts == "Prior Workout")
			showMessage("Prior workouts detected.")
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
	else
		showMessage("No workouts found this time!")
	endIf
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
	showMessage("Congratulations.\nYou have reached level " + currentLevel + "!")
	showMessage("Health (+" + health + ")\nStamina (+" + stamina + ")\nMagicka (+" + magicka + ")")
endFunction

function showMessage(string msg)
	debug.messageBox(msg)
	Utility.wait(0.1)
endFunction

;update the xp bar to show the progress gained
function updateXpBar(string levelUpsString)
	showMessage(outstandingLevel)
	float outstandingHealth = getLevelComponent(levelUpsString,0,"H")
	float outstandingStamina = getLevelComponent(levelUpsString,0,"S")
	float outstandingMagicka = getLevelComponent(levelUpsString,0,"M")
	float outstandingWeight = outstandingHealth + outstandingStamina + outstandingMagicka
	Game.setPlayerExperience(outstandingWeight)
endFunction