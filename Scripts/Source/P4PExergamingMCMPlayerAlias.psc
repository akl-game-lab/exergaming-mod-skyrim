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

int pollDuration = 120
float pollInterval = 0.5
int pollCount = 1
int producerIndex = 0
int consumerIndex = 0
string[] messageList

event OnPlayerLoadGame()
	creationDate = currentDate()
	clearDebug()
	messageList = new string[100]
	forceFetchMade = false
	oldSaveLoaded = false
	if (syncedUserName != "")
		showDebugMessage("Currently synced with " + syncedUserName)
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		if(isOldSave(creationDate))
			oldSaveLoaded = true
		endIf
		startNormalFetch("Skyrim",syncedUserName)
		normalFetchMade = true
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif
	RegisterForUpdate(pollInterval)
endEvent

event onUpdate()
	;if next position of array is not empty, 
	if(producerIndex > consumerIndex)
		debug.messageBox(messageList[consumerIndex])
		;ShowMessage(messageList[consumerIndex], false, "$Ok")
		consumerIndex = consumerIndex + 1
	endIf
	if(saveRequested == true)
		saveRequested = false
		Game.requestSave()
	endIf
	if (normalFetchMade == true && pollCount % 6 == 0)
		pollCount = 1;
		if (0 < getRawDataWorkoutCount())
			if(oldSaveLoaded)
				getLevelUps(getWorkoutsFromBestWeek(creationDate))
			else
				getLevelUps(getWorkoutsString(Game.getPlayer().getLevel()))
			endIf
			forceFetchMade = false
		elseIf (forceFetchMade == false)
			showDebugMessage("No workouts found.")
		endIf
		normalFetchMade = false
	endIf
	if (forceFetchMade == true && currentDate() - pollStartTime == pollDuration)
		showDebugMessage("Search for recent workouts complete.");
		forceFetchMade = false
		startNormalFetch("Skyrim",syncedUserName)
		normalFetchMade = true
	endIf
	pollCount = pollCount + 1
endEvent

function getLevelUps(string workouts)
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
	messageList[producerIndex] = msg
	producerIndex = producerIndex + 1
endFunction

;update the xp bar to show the progress gained
function updateXpBar(string levelUpsString)
	float outstandingHealth = getLevelComponent(levelUpsString,0,"H")
	float outstandingStamina = getLevelComponent(levelUpsString,0,"S")
	float outstandingMagicka = getLevelComponent(levelUpsString,0,"M")
	float outstandingWeight = outstandingHealth + outstandingStamina + outstandingMagicka
	Game.setPlayerExperience(outstandingWeight)
endFunction