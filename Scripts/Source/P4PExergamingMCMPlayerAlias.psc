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

event OnPlayerLoadGame()
	clearDebug()
	pollStartTime = 0
	forceFetchMade = false
	oldSaveLoaded = false
	if (syncedUserName != "")
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		oldSaveLoaded = isOldSave(creationDate as int)
		startNormalFetch("Skyrim",syncedUserName)
		normalFetchMade = true
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif
	RegisterForUpdate(pollInterval)
endEvent

event onUpdate()
	int pollDuration = 120
	
	if(saveRequested == true)
		creationDate = currentDate()
		saveRequested = false
		Utility.WaitMenuMode(1)
		Game.requestSave()
	endIf
	if (normalFetchMade == true && pollCount % 6 == 0)
		pollCount = 1;
		if(oldSaveLoaded == true)
			getLevelUps(getWorkoutsFromBestWeek(creationDate))
		elseIf (0 < getRawDataWorkoutCount())
			getLevelUps(getWorkoutsString(Game.getPlayer().getLevel()))
			forceFetchMade = false
		elseIf (forceFetchMade == false)
			noWorkoutsFound.show()
		endIf
		normalFetchMade = false
	endIf
	if (forceFetchMade == true)
		debug.Notification("Checking for recent workouts.")
		int elapsed = currentDate() - pollStartTime
		if(elapsed >= pollDuration)
			searchComplete.show()
			forceFetchMade = false
			startNormalFetch("Skyrim",syncedUserName)
			normalFetchMade = true
		endIf
	endIf
	pollCount = pollCount + 1
endEvent

function getLevelUps(string workouts)
	if(workouts == "Prior Workout")
		priorWorkouts.show()
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
	levelUpMessage.show(currentLevel)
	levelUpDetails.show(health,stamina,magicka)
endFunction

;update the xp bar to show the progress gained
function updateXpBar(string levelUpsString)
	float outstandingHealth = getLevelComponent(levelUpsString,0,"H")
	float outstandingStamina = getLevelComponent(levelUpsString,0,"S")
	float outstandingMagicka = getLevelComponent(levelUpsString,0,"M")
	float outstandingWeight = outstandingHealth + outstandingStamina + outstandingMagicka
	;display message for progress to next level
	;first progress, second amount of workout
	levelProgressMsg.show(outstandingWeight*100, 20)
	Game.setPlayerExperience(outstandingWeight)
endFunction