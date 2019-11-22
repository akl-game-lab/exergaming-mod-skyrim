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
bool property isNewGame = True auto

bool property manageOwnStats auto
int property availablePoints auto
int property healthPoints auto
int property magickaPoints auto
int property staminaPoints auto

message property noWorkoutsFound auto
message property searchComplete auto
message property priorWorkouts auto
message property levelUpMessage auto
message property levelUpDetails auto
message property levelProgressMsg auto
message property ExergamingLevelUpMessageManagingOwn auto

;Event log file
String eventLog = "SkyrimExergameMod_EventLog"

float pollInterval = 0.5
int pollCount = 1
int searchCount = 1
int delayCount = 0

;Resets variables used for leveling or polling
function initialise()
	Debug.OpenUserLog(eventLog)
	clearDebug()
	pollCount = 1
	pollStartTime = 0
	forceFetchMade = false
	oldSaveLoaded = false
	searchCount = 1
	RegisterForUpdate(pollInterval)
	
	Debug.TraceUser(eventLog, "120 Mod Turned On", 0)
	Debug.TraceUser(eventLog, "isNewGame? :" +isNewGame, 0)

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
	
	Debug.TraceUser(eventLog, "121 Mod Turned Off", 0)
	
endFunction

;Executes when a save finishes loading up
event OnPlayerLoadGame()
	initialise()
	if (syncedUserName != "");Check to see if user is synced with an account
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)
		Debug.TraceUser(eventLog, "128 Checking if save is old, Creation Date is: " +creationDate, 0)
		oldSaveLoaded = isOldSave(creationDate as int)
		;If the game has no prior saves the creation date will be 0, this will cause a 'Best week' levelup on next load
		;Handle this by checking the isNewGame flag and correcting the oldSaveLoadedFlag
		if(isNewGame)
			oldSaveLoaded = False
		EndIf
		Debug.TraceUser(eventLog, "122 Save Game loaded. File is an old save: " +oldSaveLoaded, 0)
		
		startNormalFetchWithErrorHandling()
	else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
		
		Debug.TraceUser(eventLog, "123 Save Game loaded. No user synced", 0)
		
	endif
endEvent

;Executes automatically every second, called by the game
event onUpdate()
	int pollDuration = 30

	
	if(saveRequested == true)
		creationDate = currentDate()
		Debug.TraceUser(eventLog, "126 CreationDate of save is: " +creationDate, 0)
		saveRequested = false
		Utility.WaitMenuMode(1)
		Game.requestSave()
		updateConfig()
		isNewGame = False
		Debug.TraceUser(eventLog, "129 isNewGame: " +isNewGame, 0)
		
		Debug.TraceUser(eventLog, "101 Game Saved", 0)
		
	endIf

	if ((normalFetchMade == true) && mod(pollCount,6) == 0)
		pollCount = 1
		normalFetchMade = false
		if(oldSaveLoaded == true)
			getLevelUps(getWorkoutsFromBestWeek(creationDate))
			
			Debug.TraceUser(eventLog, "102 Retreiving work outs from best week as this is an old save", 0)
			Debug.TraceUser(eventLog, "127 CreationDate of save is: " +creationDate, 0)
			
		elseIf (0 < getRawDataWorkoutCount());force fetch returned data
			getLevelUps(getWorkoutsString(Game.getPlayer().getLevel()))
			Utility.wait(2.0)
			forceFetchMade = false
			
			Debug.TraceUser(eventLog, "103 Force fetch returned data", 0)
			
		elseIf (forceFetchMade == false)
			noWorkoutsFound.show()
			
			Debug.TraceUser(eventLog, "104 No workouts found", 0)
			
		endIf
	endIf

	if (forceFetchMade == true);

		if (delaycount ==  8) 

			searchCount = Utility.RandomInt(0,22)

			if (searchCount == 0)

				debug.Notification("Talking to your personal trainer")

			ElseIf (searchCount == 1)
				
				debug.Notification("Smelling your gym socks")

			ElseIf (searchCount == 2)
				
				debug.Notification("Looking at your gym's CCTV footage")

			ElseIf (searchCount == 3)

				debug.Notification("Comparing alibis")

			ElseIf (searchCount == 4)

				debug.Notification("Checking popular bodybuilding forums")

			ElseIf (searchCount == 5)

				debug.Notification("Hiring forensic investigators")

			ElseIf (searchCount == 6)

				debug.Notification("Looking for workouts")

			ElseIf (searchCount == 7)

				debug.Notification("Probing the host")

			ElseIf (searchCount == 8)

				debug.Notification("...")

			ElseIf (searchCount == 9)

				debug.Notification("Hitting a new deadlift PB")

			ElseIf (searchCount == 10)

				debug.Notification("Hitting a new squat PB")

			ElseIf (searchCount == 11)

				debug.Notification("Hitting a new bench press PB")

			ElseIf (searchCount == 12)

				debug.Notification("Tying shoelaces")

			ElseIf (searchCount == 13)

				debug.Notification("Defrosting chicken breast")

			ElseIf (searchCount == 14)

				debug.Notification("Waiting patiently")

			ElseIf (searchCount == 15)

				debug.Notification("Smiling at the gym receptionist")

			ElseIf (searchCount == 16)

				debug.Notification("Querying server")

			ElseIf (searchCount == 17)

				debug.Notification("Decoding transmission")

			ElseIf (searchCount == 18)

				debug.Notification("Unracking weights")

			ElseIf (searchCount == 19)

				debug.Notification("Completing set")

			ElseIf (searchCount == 20)

				debug.Notification("Stretching")

			ElseIf (searchCount == 21)

				debug.Notification("Flexing")

			ElseIf (searchCount == 22)

				debug.Notification("Scanning our server")
			
			Else
				
				debug.Notification("Scanning our server")

			endif;

			delayCount = 0

		else 

			delayCount = delayCount + 1

		endif

		int elapsed = currentDate() - pollStartTime
		if(elapsed >= pollDuration)
			searchComplete.show()
			forceFetchMade = false
			
			Debug.TraceUser(eventLog, "106 Fetch from exercise.com Complete, retrieving data from our server", 0)
			
			startNormalFetchWithErrorHandling()
		endIf
	endIf
	pollCount = pollCount + 1
endEvent

;Starts a normal fetch and handles the response to provide the user with error feedback
function startNormalFetchWithErrorHandling()
	int serverResponse = startNormalFetch("Skyrim",syncedUserName)
	Debug.TraceUser(eventLog, "124 Normal Fetch Started", 0)
	if( serverResponse == 404)
		debug.messageBox("Server Invalid State Error\n\nPlease contact support with the current date and time.\n\n Code: 107")
		
		Debug.TraceUser(eventLog, "107 Server Returned: " +serverResponse, 1)
		
	elseIf( serverResponse == 400 )
		debug.messageBox("Server Config Error\n\nPlease contact support with the current date and time.\n\n Code: 108")
		
		Debug.TraceUser(eventLog, "108 Server Returned: " +serverResponse, 1)
		
	elseIf( serverResponse == 200)
		normalFetchMade = true
		
		Debug.TraceUser(eventLog, "109 Server Returned: " +serverResponse, 0)
		Debug.TraceUser(eventLog, "110 Success", 0)
		
	elseIf( serverResponse == 503)
		debug.messageBox("Server Unreachable\n\nPlease try again in a few minutes.\n\nIf this error persists, please contact support with the current date and time.\n\n Code: 107")
		
		Debug.TraceUser(eventLog, "107 Server Unreachable, Returned: " +serverResponse, 1)

	else
		debug.messageBox("Cannot Contact Server\n\nPlease try again in a few minutes.\n\nIf this error persists, please contact support with the current date and time.\n\nCode: 111")
		
		Debug.TraceUser(eventLog, "111 Unforseen error, Server Returned: " +serverResponse, 2)
		
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
		debug.messageBox("No new Workouts found\n\n Workouts prior to the mod being turned on are ineligible")
		
		Debug.TraceUser(eventLog, "112 The only work outs found were logged before the mod was turned on for a date prior to the mod being turned on, No level up applied", 0)
		
	;Special case when workouts are found when the mod is turned on
	elseIf(workouts == "Prior Workout")
		
		Debug.TraceUser(eventLog, "113 Workouts logged prior to mod being turned on, generic level up being applied", 0)
		
		priorWorkouts.show()
		;--------------------------------------------------
		levelsUp = 1
		healthUp = 4
		staminaUp = 3
		magickaUp = 3
		;--------------------------------------------------
		doLevelUp(levelsup, healthUp,staminaUp,magickaUp)
		levelUpsString = "0,0,0;4,3,3"
	;General case
	else
		
		Debug.TraceUser(eventLog, "114 Calculating Level up", 0)
		levelUpsString = getLevelUpsAsString(outstandingLevel,workouts)
		;Level ups start at index 1 as index 0 holds the outstanding level up
		int n = 1
		bool shouldContinue = isNthLevelUp(levelUpsString,n)
		while (shouldContinue)
			int health = getLevelComponent(levelUpsString,n,"H")
			int stamina = getLevelComponent(levelUpsString,n,"S")
			int magicka = getLevelComponent(levelUpsString,n,"M")

			Debug.TraceUser(eventLog, "115.0 Applying Level up", 0)
			Debug.TraceUser(eventLog, "115.1 Health: "+health, 0)
			Debug.TraceUser(eventLog, "115.1 Stamina: "+stamina, 0)
			Debug.TraceUser(eventLog, "115.3 Magicka: "+magicka, 0)
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
		Debug.TraceUser(eventLog, "116 Outstanding Level: "+outstandingLevel, 0)
	endIf
	float multiplier = 1
	if (levelsup > 3)
		multiplier = 3/(levelsup as float)
		debug.traceuser(eventlog, levelsup + " " + healthUp + " " + staminaup + " " + magickaup, 0)
		debug.traceuser(multiplier, 0)
		levelsup = 3
	endif
	debug.TraceUser(eventLog, multiplier, 0)
	healthup = (healthup * multiplier) as int
	staminaup = (staminaUp * multiplier) as int
	magickaup = (magickaup * multiplier) as int

	Debug.TraceUser(eventLog, "117.0 Updating XP bar, levelUpsString: "+levelUpsString, 0)
	Debug.TraceUser(eventLog, "117.1 Updating XP bar, healthup: "+healthUp, 0)
	Debug.TraceUser(eventLog, "117.2 Updating XP bar, staminaup: "+staminaUp, 0)
	Debug.TraceUser(eventLog, "117.3 Updating XP bar, magickaup: "+magickaUp, 0)
	if(manageOwnStats)
		magickaup = magickaPoints * levelsUp
		healthUp = healthPoints * levelsUp
		staminaUp = staminaPoints * levelsUp
	endif
	doLevelUp(levelsUp, healthUp, staminaUp, magickaUp)
	updateXpBar(levelUpsString, levelsUp, healthUp, staminaUp, magickaUp)
	saveRequested = true
endFunction

;Increment the player level and give the player a perk point
function doLevelUp(int level, int health, int stamina, int magicka)
	
	Actor player = Game.getPlayer()
	int currentLevel = player.getLevel()

	Debug.TraceUser(eventLog, "118 Doing Level up, Current Level (preUp): " +currentLevel, 0)

	int carryCapacityUp = (stamina/2)  	;The division function is not working. this seems to work okay as the numbers are whole and simple.
	
	float currenthealth = Game.GetPlayer().GetBaseActorValue("health")
	float currentstamina = Game.getPlayer().getBaseActorValue("stamina")
	float currentmagicka = Game.getPlayer().getBaseActorValue("magicka")
	float currentcarryweight = Game.getPlayer().getBaseActorValue("CarryWeight")

	player.setActorValue("health", currenthealth + health)
	player.setActorValue("stamina", currentstamina + stamina)
	player.setActorValue("magicka", currentmagicka + magicka)
	player.setActorValue("CarryWeight", currentcarryweight + carryCapacityUp) ;Added to increase carry weight proportionally with stamina as in the vanilla version
	Game.setPlayerLevel(currentLevel + level)
	currentLevel = player.getLevel()
	Game.setPerkPoints(Game.getPerkPoints() + level)

	Debug.TraceUser(eventLog, "119.0 LevelUp Done: New Current Level: " +currentLevel, 0)
	Debug.TraceUser(eventLog, "119.1 LevelUp Done: Health Increase: " +health, 0)
	Debug.TraceUser(eventLog, "119.2 LevelUp Done: Magicka Increase: " +magicka, 0)
	Debug.TraceUser(eventLog, "119.3 LevelUp Done: Stamina Increase: " +stamina, 0)
	Debug.TraceUser(eventLog, "119.4 LevelUp Done: Carry Capacity increase: " +carryCapacityUp, 0)
	Debug.TraceUser(eventLog, "119.5 LevelUp Done: Perkpoints incresed by One, New value: " +Game.GetPerkPoints(), 0)
	
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
		if(manageOwnStats)
			levelUpDetails.show(levelsUp, Game.getPlayer().getLevel())
		else
			levelUpMessage.show(levelsUp,Game.getPlayer().getLevel(),magickaUp, healthUp,staminaUp)
		endif
		debug.Notification("Remember to allocate perk points in the skills menu")
	endIf
	if(outstandingWeight > 0)
		levelProgressMsg.show(outstandingWeight, getPointsToNextLevel(outstandingWeight))
	endIf
	Game.setPlayerExperience(Game.getExperienceForLevel(Game.getPlayer().getLevel())*(outstandingWeight/100))
endFunction