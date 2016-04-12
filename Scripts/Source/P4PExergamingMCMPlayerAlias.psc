Scriptname P4PExergamingMCMPlayerAlias extends ReferenceAlias  

import myPluginScript
import FISSFactory
import StringUtil

int Property RealPlayerLevel Auto
Potion Property LevelUpPotion Auto
string property syncedUserName auto
int property syncedSaveID auto

;message 1
message property notEnoughExp auto
;message 2
message property levelUpMessage auto
int messageVariable ;number to be displayed in the message
int messageChoice ;either 1 or 2


;this array will keep track of which days in the first week the user has received a potion
;this is needed because the user is only meant to receive 1 potion per workout per day in the first week
bool[] firstWeekPotionReceived
int firstWeekPotions
string firstSyncDate
string lastSyncDate
int totalPoints
int totalWorkouts
bool firstWeekCompleted
int strengthPoints
int fitnessPoints
int sportsPoints
string stackedLevelDistributions
FISSInterface fiss

Event OnInit()
	firstWeekPotionReceived = new bool[7]
EndEvent

Event OnPlayerLoadGame()
	;when the player loads in, need to grab the previous exercisedata if there is a synced account
	debug.messagebox("The synced user is: " + syncedUserName + "the theoretical level is: " + realPlayerLevel)
	if (syncedUserName != "")
		Game.SetGameSettingFloat("fXPPerSkillRank", 0)

		fiss = FISSFactory.getFISS()
		If !fiss
			debug.MessageBox("Fiss is not installed, Please install Fiss before using this mod.")
			return
		endif

		;load the old exercisedata file
		loadOldData()

		; debug.messagebox("the last sync date is: " + lastSyncDate)
		;save the xml from the server into the fiss folder
		saveXml()

		;here we now read the xml file, and add its values to the past values we have read
		;need to process the new data eg. check for first week points and such. then add it to the *points variables.
		importDataFromXml()

		;need to process the new data eg. check for first week points and such. then add it to the *points variables.

		;before converting to potions, the fxplevelupmult variables have to be set.
		setExpVariables()

		;Then we loop through the variables and create a max of 3 potions from them. just like in the previous code used in the 
		;player reference alias script. (onplayerloadgame)
		convertExpToPotions()

		;saves the data back into the file
		updateExerciseDataFile()

		;changes the player's exp bar to match their required experience
		UpdateExperienceProgressBar()
		
		displayEndingMessage()
		game.requestSave()
	Else
		Game.SetGameSettingFloat("fXPPerSkillRank", 1)
	endif

endEvent

;###################################################
;METHODS
;###################################################

;reads the previous data is the players exerciseData.txt file and loads the values
Function loadOldData()
	;get all the past data from the save file
	fiss.beginLoad( syncedUserName + syncedSaveID + "_Exercise_Data.txt")
	firstSyncDate = fiss.loadString("first_import_date")
	lastSyncDate = fiss.loadString("last_import_date")
	totalPoints = fiss.loadInt("total_points")
	totalWorkouts = fiss.loadInt("total_workouts")

	firstWeekCompleted = fiss.loadBool("first_week_completed")
	strengthPoints = fiss.loadInt("outstanding_strength_points")
	fitnessPoints = fiss.loadInt("outstanding_fitness_points")
	sportsPoints = fiss.loadInt("outstanding_sports_points")

	fiss.endLoad()
endFunction

;runs the custom SKSE functions to save the xml into the skyrim directory
Function saveXml()
	string year = subString(lastSyncDate, 0, 4)
	string month = subString(lastSyncDate, 4, 2)
	string dayhour = subString(lastSyncDate, 6, 4)
	string minute = subString(lastSyncDate, 10, 2)
	string second = subString(lastSyncDate, 12, 2)
	string formattedDate = year + "-" + month + "-" + dayhour + ":" + minute + ":" + second
	; debug.messagebox("the formatted date is: " + formattedDate)
	; debug.messagebox("the syncedUserName is: " + syncedUserName)
	debug.messagebox("fetchxml returns: " + fetchXml("Skyrim", syncedUserName, formattedDate))
endFunction


;assumes the updated xml file has already been saved into the fiss directory and then loads the data.
Function importDataFromXml()
	fiss.beginLoad("NEWXMLDATA.txt")
	stackedLevelDistributions = ""
	int workoutCounter = 1
	string currentWorkout = fiss.loadString("workout" + workoutCounter)
	string timeStamp = ""
	string statDistribution = ""
	int staminaIndex = 0
	int magickaIndex = 0
	int health = 0
	int stamina = 0
	int magicka = 0
	firstWeekPotions = 0

	while(isPrintable(currentWorkout))
		debug.messageBox("This is the workout string: " + currentWorkout)

		;get the timeStamp of the workout
		;will need to change the index values if the format of the timestamp changes.
		timestamp = subString(currentWorkout, 0, 14)

		;check if the first week is completed
		;this section has a problem due to not being able to store the time stamps in the float type because of precision error
		if(!firstWeekCompleted)
			if (isFirstWeekCompleted(firstSyncDate, timestamp))
				firstWeekCompleted = true
				debug.messageBox("TRUE")
			endIf
		endIf

		;get the point values for each of the stats
		staminaIndex = find(currentWorkout, "s")
		magickaIndex = find(currentWorkout, "m")

		health = (subString(currentWorkout, 16, staminaIndex - 16)) as int
		stamina = (subString(currentWorkout, staminaIndex + 1, magickaIndex - staminaIndex - 1)) as int
		magicka = (subString(currentWorkout, magickaIndex + 1)) as int

		;check here if still in first week, then give a potion for the workout.
		if (!firstWeekCompleted)
			int firstWeekWorkoutDay = dayOfTheWeek(firstSyncDate, timestamp)
			if (!firstWeekPotionReceived[firstWeekWorkoutDay])
				firstWeekPotionReceived[firstWeekWorkoutDay] = true
				firstWeekPotions = FirstWeekPotions + 1
				incrementRealPlayerLevel()
				stackedLevelDistributions = stackedLevelDistributions + calculateLevelDist(health, stamina, magicka)

			endIf
		else

			;add the workout values to the respective variables
			strengthPoints = strengthPoints + health
			fitnessPoints = fitnessPoints + stamina
			sportsPoints = sportsPoints + magicka

		endIf


		;add all the stats of this workout to the total stat count
		totalPoints = totalPoints + health + stamina + magicka
		workoutCounter = workoutCounter + 1
		currentWorkout = fiss.loadString("workout" + workoutCounter)

	endWhile

	fiss.endload()

	;add the number of workouts ot the total workouts
	totalWorkouts = totalWorkouts + workoutCounter - 1
	
	;set last sync to currentDate
	lastSyncDate = currentDate()

endFunction

;this function is used for the first week potions and distributes the stats depending on what exercise the user did.
string Function calculateLevelDist(int health, int stamina, int magicka) 
	int total = health + stamina + magicka
	int healthPoints = (health*10)/total
	int staminaPoints = (stamina * 10)/total
	int magickaPoints = 10 - healthPoints - staminaPoints ;"h10 s00 m00"
	string levelDist
	if (healthPoints == 10)
		levelDist = "h10 s00 m00"
	elseif(staminaPoints == 10)
		levelDist = "h00 s10 m00"
	ElseIf(magickaPoints == 10)
		levelDist = "h00 s00 m10"
	else
		levelDist = "h0" + healthPoints + " s0" + staminaPoints + " m0" + magickaPoints
	endIf
	return levelDist
endFunction


;This function sets the exp variables, so that the exp required calculations are scaled to the exercise.com points.
Function setExpVariables()
	if (totalWorkouts == 0)
		Game.SetGameSettingFloat("fXPLevelUpBase", 1000)
	else
		Game.SetGameSettingFloat("fXPLevelUpBase", (totalPoints/totalWorkouts))
	endIf
	Game.SetGameSettingFloat("fXPLevelUpMult", (25))
EndFunction



;Award the player Potions of Gains if they have enough experience to level up
Function convertExpToPotions()
	;This string stores arrays which indicate the distribution of attribute points to award for each Potion of Experience
	;The strings will be in the format of
	;		hX sY mZ 		where X, Y, Z are non-negative integers
	float requiredXPToLevelUp = Game.GetExperienceForLevel(realPlayerLevel)
	float totalExpRequiredForLevel = requiredXPToLevelUp
	
	;Duplicated the fields as we do not want threading to break the logic
	int strengthPointsCopy = strengthPoints
	int fitnessPointsCopy = fitnessPoints
	int sportsPointsCopy = sportsPoints
	debug.messageBox(strengthPoints + " " + fitnessPoints + " " + sportsPoints)
	;StackLevels is the number of pending levels the player will have. Increment this whenever the player has enough XP to earn another level up.
	int stackLevels = firstWeekPotions
	int remainingPoints = strengthPointsCopy + fitnessPointsCopy + sportsPointsCopy

	int healthStats = 0
	int staminaStats = 0
	int magickaStats = 0
	string levelDist = ""
	
	int count = firstWeekPotions
	bool levelComplete = False


	;As long as the total amount of experience that hasn't been "used" is greater than 0, go through and check if the player can get any Potions of Gains.
	;There is also a check for how many times it has been looped through with count, and caps at 3 times to prevent the player from leveling too much.
	; debug.messageBox("required xp : " + requiredXPToLevelUp)
	; debug.messageBox("remainingpoints: " + remainingpoints)
	While (remainingPoints >= requiredXPToLevelUp) && (count < 3)
		debug.messageBox("required xp : " + requiredXPToLevelUp)
		If (strengthPointsCopy >= requiredXPToLevelUp)
			;if there is enough strength points to level
			levelDist = "h10 s00 m00"
			stackedLevelDistributions = stackedLevelDistributions + levelDist
			stackLevels = stackLevels + 1

			remainingPoints = remainingPoints - requiredXPToLevelUp as int
			strengthPointsCopy = strengthPointsCopy - requiredXPToLevelUp as int

			levelComplete = True

		ElseIf levelComplete != true
			float healthPercentage = strengthPointsCopy/totalExpRequiredForLevel
			healthStats = (10*healthPercentage) as int

			requiredXPToLevelUp = requiredXPToLevelUp - strengthPointsCopy	
			remainingPoints = remainingPoints - strengthPointsCopy 
		EndIf

		If (!levelComplete) && (fitnessPointsCopy >= requiredXPToLevelUp)
			int remainingStats = (10 - healthStats) as int
			staminaStats = remainingStats

			if (staminaStats == 10)
				levelDist = "h00 s10 m00"
			else
				levelDist = "h0"+ healthStats + " s0" + staminaStats + " m00"
			endIf

			stackedLevelDistributions = stackedLevelDistributions + levelDist
			stackLevels = stackLevels + 1

			remainingPoints = remainingPoints - requiredXPToLevelUp as int
			fitnessPointsCopy = fitnessPointsCopy - requiredXPToLevelUp as int
			levelComplete = True

			strengthPointsCopy = 0
		ElseIf (!levelComplete)
			float staminaPercentage = fitnessPointsCopy/totalExpRequiredForLevel
			staminaStats = (10*staminaPercentage) as int

			requiredXPToLevelUp = requiredXPToLevelUp - fitnessPointsCopy 
			remainingPoints = remainingPoints - fitnessPointsCopy
		EndIf

		If (!levelComplete) && (sportsPointsCopy >= requiredXPToLevelUp)
			int remainingStats = 10 - healthStats - staminaStats
			magickaStats =  remainingStats

			if (magickaStats == 10)
				levelDist = "h00 s00 m10"
			else
				levelDist = "h0"+ healthStats + " s0" + staminaStats + " m0" + magickaStats
			endIf

			stackedLevelDistributions = stackedLevelDistributions + levelDist
			stackLevels = stackLevels + 1

			remainingPoints = remainingPoints - requiredXPToLevelUp as int
			sportsPointsCopy = sportsPointsCopy - requiredXPToLevelUp as int

			levelComplete = True

			strengthPointsCopy = 0
			fitnessPointsCopy = 0
		ElseIf (!levelComplete)
			;this is reached when not enough points gathered to level
			count = 3
		EndIf
		
		if (levelComplete)
		
			incrementRealPlayerLevel()
			requiredXPToLevelUp = Game.GetExperienceForLevel(realPlayerLevel)
			count = count + 1
			levelComplete = False
			healthStats = 0
			staminaStats = 0
			magickaStats = 0

	 	EndIf
	EndWhile

	debug.MessageBox("the stacked Level Distribution is: " + stackedLevelDistributions)
	if (stackLevels == 0)
		;Debug.MessageBox("No potions this time. Go harder in the gym next time! You still need " + ((requiredXPToLevelUp - remainingpoints) as int) + " XP to level up.")
		messageChoice = 1
		messageVariable = ((requiredXPToLevelUp - remainingpoints) as int)
	else
		;Debug.MessageBox("Congratulations you have gained " + stackLevels + " Potion(s) of Gains. Use them to level up.")
		messageChoice = 2
		messageVariable = stackLevels
		;Add Potion(s) of Experience to the player's inventory. The number of orbs given is the number of stackLevels.
		game.getplayer().AddItem(LevelUpPotion, stackLevels)
	
		;Write the attribute distributions to a file so that they can be read in after X amount of time (see quest script) when an Orb of Experience is consumed.
		writeDistsToFile(stackedLevelDistributions)
	
		;if 3 potions were created, then clear all of the workout points that the player currently has to promote the player to still play the game frequently
		if (stackLevels == 3) 
			strengthPoints = 0
			fitnessPoints = 0
			sportsPoints = 0
		else
			;Otherwise just map the points variables we manipulated to the points fields
			strengthPoints = strengthPointsCopy
			fitnessPoints = fitnessPointsCopy
			sportsPoints = sportsPointsCopy
		endIf
	EndIf
EndFunction

;This function is used to increment the realPlayerLevel variable which can also be accessed from outside this script through this function.
;This function is called anytime the player gains a level, so it will also check to see which level bracket the player should be in.
Function incrementRealPlayerLevel()
	realPlayerLevel = realPlayerLevel + 1
	setExpVariables()
EndFunction

;NOTE: This method was used before when there were exp brackets, but for now we are testing it without
;an exp bracket, so this method is not used at the moment.
;Similar to the above method but this checks whether a player has entered a new bracket
;after levelling up, such as going from level 9 to 10.
;only works for single level increments
Function UpdateLevelModifiers(int newPlayerLevel)
	float levelUpBase = 0
	float levelUpMultiplier = 0
	int bracketChangedTo = 1

	;Check which level bracket the player has changed into
	If (newPlayerLevel == 10)
		bracketChangedTo = 2
	ElseIf (newPlayerLevel == 20)
		bracketChangedTo = 3
	ElseIf (newPlayerLevel ==30)
		bracketChangedTo = 4
	EndIf

	If bracketChangedTo != 1
		
		fiss.beginLoad(syncedUserName + syncedSaveID + "_Exercise_Data.txt")
		totalPoints = fiss.loadInt("total_points")

		;this is an error checking to make sure that no infinite level ups occur.
		if (totalPoints == 0)
			totalPoints = 411
		EndIf

		;Using which bracket the player has switched to, recalculate the player's experience requirement
		If bracketChangedTo == 2
			float temp = totalPoints * 2.6666666667
			levelUpBase = temp/22.154
			levelUpMultiplier = temp/288.0
		ElseIf bracketChangedTo == 3
			float temp = totalPoints * 2.6666666667 * 1.25
			levelUpBase = temp/17.143
			levelUpMultiplier = temp/600.0
		ElseIf bracketChangedTo == 4
			float temp = totalPoints * 2.6666666667 * 1.25 * 1.4
			levelUpBase = temp/16.5
			levelUpMultiplier = temp/1237.5
		EndIf

		Game.SetGameSettingFloat("fXPLevelUpBase", levelUpBase)
		Game.SetGameSettingFloat("fXPLevelUpMult", levelUpMultiplier)
	else
		Game.SetGameSettingFloat("fXPLevelUpBase", (411/(540/16)))
		Game.SetGameSettingFloat("fXPLevelUpMult", (411/(540/10)))
	endIf

EndFunction

;Write attribute distributions to a file so that they may be read in when a potion is used is used
Function writeDistsToFile(string levelDists)

	String previousDistributions = ""
	;check if the distributions file exists
	fiss.beginLoad(syncedUserName + syncedSaveID + "_StackedLevelDistributions.txt")
	string readInSuccess = fiss.endLoad()

	if (readInSuccess == "")
		;file is found so we need to load the previous distributions
		fiss.beginLoad(syncedUserName + syncedSaveID + "_StackedLevelDistributions.txt")
		previousDistributions = fiss.loadString("Level_Distributions")
		fiss.endLoad()
		if ( IsPunctuation(previousDistributions))
			previousDistributions = ""
		EndIf

	endIf

	fiss.beginSave(syncedUserName + syncedSaveID + "_StackedLevelDistributions.txt", "P4P")
	levelDists =  previousDistributions + levelDists
	fiss.saveString("Level_Distributions", levelDists)
	fiss.endsave()
EndFunction

;Update the exercise_data file after points have been spent to ensure players cannot get multiple level ups from the same exercise session points
Function updateExerciseDataFile()

	fiss.beginSave(syncedUserName + syncedSaveID + "_Exercise_Data.txt", "P4P")

	fiss.saveString("first_import_date", firstSyncDate)
	fiss.saveString("last_import_date", lastSyncDate)
	fiss.saveInt("total_points", totalPoints)
	fiss.saveInt("total_workouts", totalWorkouts)
	fiss.saveString("first_week_completed", firstWeekCompleted)

	fiss.saveString("outstanding_strength_points", strengthPoints)
	fiss.saveString("outstanding_fitness_points", fitnessPoints)
	fiss.saveString("outstanding_sports_points", sportsPoints)

	string endWrite = fiss.endSave()
EndFunction

;Set the player's experience points to their progress bar
;If they have received any Orbs of Experience then the progress bar should show up as full, and if not it should 
;be the amount they have progressed towards the next level
Function UpdateExperienceProgressBar()
	Actor player = Game.GetPlayer()
	float ExperienceBarMaxValue = Game.GetExperienceForLevel(player.getLevel())
	float requiredXPToLevelUp =  Game.GetExperienceForLevel(realPlayerLevel)
	float currentExp = strengthPoints + fitnessPoints + sportsPoints
	float scaledExp = (currentExp * ExperienceBarMaxValue) / requiredXPToLevelUp
	Game.SetPlayerExperience(scaledExp)
EndFunction

;This method is just used for debugging to check the values of the exerciseData dynamically
Function printValues()
	debug.messageBox("The first Import Date is: " + firstSyncDate)
	debug.messageBox("The last Import Date is: " + lastSyncDate)
	debug.messageBox("The total number of points earned is: " + totalPoints)
	debug.messageBox("The total number of workouts done is: " + totalWorkouts)
	debug.messageBox("The first week completed status is: " + firstWeekCompleted)
	debug.messageBox("The strength points is: " + strengthPoints)
	debug.messageBox("The fitness points is: " + fitnessPoints)
	debug.messageBox("The sports points is: " + sportsPoints)
endFunction

;this message needs to be displayed last, because when a save is requested, it will only be reliable if it
;occurs after a message.show funciton call. For more information look on the pivotal tracker story called
;"need to save after editing the textfile"
Function displayEndingMessage()
	if (messageChoice == 1)
		notEnoughExp.show(messageVariable)
	else
		levelUpMessage.show(messageVariable)
	endIf
endFunction