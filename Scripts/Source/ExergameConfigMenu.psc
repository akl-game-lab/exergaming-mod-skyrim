Scriptname ExergameConfigMenu extends ski_configbase  

import PluginScript
import StringUtil

;reference to the playerreference script
P4PExergamingMCMPlayerAlias property playerReference auto

;option IDs
int exergameModOnSwitch
int exergameModOffSwitch
int syncStatus
int forceFetchStatus
int forceFetchButton

;option values
bool exergameModOn = false
bool forceFetch = false

;Defines the number of pages in the MCM
event OnConfigInit()
	Pages = new string[1]
	Pages[0] = "Settings"
endEvent

;Shows either the exergaming splash screen or the settings page
event OnPageReset(string page)
	;Splash screen
	if (page == "")
		LoadCustomContent("Exergame_Menu.dds",0,24)
	;Settings page
	elseif (page == "Settings")
		UnloadCustomContent()
		;initial options setup
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)
		AddHeaderOption("Exergame Mod")
		
		string syncedUserName = playerReference.syncedUserName
		
		if(syncedUserName != "")
			exergameModOn = true;
		endIf

		if (exergameModOn)
			syncStatus = AddTextOption("Currently synced with ", syncedUserName)
			forceFetchButton = AddToggleOption("Check for recent workouts",forceFetch)
			exergameModOffSwitch = AddToggleOption("Turn Exergame Mode off", exergameModOn)
		else
			exergameModOnSwitch = AddInputOption("Turn Exergame Mod on", "");runs code in OnOptionInputOpen();
		endIf
	endIf
endEvent

;Executes when the user tries to turn the mod off
event OnOptionSelect(int option)
	if (option == exergameModOffSwitch)
		;Show the confirmation to turn off Exergaming mode (need to change this message prompt to use the implementation for a yes no message from the mcm github)
		bool turnOffExergaming = ShowMessage("Are you sure?\nThis will turn off Exergame Mod in the current save.", true, "$Yes", "$No")
		if (turnOffExergaming)
			exergameModOn = false
			playerReference.syncedUserName = ""

			;reset the exp variables
			Game.SetPlayerExperience(0)
			Game.SetGameSettingFloat("fXPPerSkillRank", 1)
			ShowMessage("Unsync complete.", false, "$Ok")
			Game.requestSave()
		endIf
	elseIf (option == forceFetchButton)
		if (playerReference.forceFetchMade == false)
			startForceFetch("Skyrim",playerReference.syncedUserName)
			debug.messageBox("A request has been made to check for recent workouts.\nThis can take up to 2 minutes.\nPress \"Check for recent workouts\" to see if a new workout has been found.")
			playerReference.forceFetchMade = true
			ForcePageReset()
		else
			checkLevelUps()
		endIf
	endIf
	ForcePageReset()
endEvent

;Executes when the user tries to turn the mod on
Event OnOptionInputOpen(int option)
	;show user username entry dialog
	SetInputDialogStartText("Please enter your username...")
EndEvent

;Executes once the user has entered a username
Event OnOptionInputAccept(int option, string userInput)
	if (option == exergameModOnSwitch)
		bool turnOnExergaming = ShowMessage("Are you sure?\nYou will not be able to gain levels in game while Exergame Mod is on.", true,  "$Yes", "$No")
		if (turnOnExergaming == true)
			string username = userInput
			if(validUsername(username))
				playerReference.syncedUserName = username
				Game.SetPlayerExperience(0)
				Game.SetGameSettingFloat("fXPPerSkillRank", 0)
				exergameModOn = true
				ForcePageReset()
				string msg = "Sync with " + username + " complete."
				ShowMessage(msg, false, "$Ok")
				Game.requestSave()
			else
				debug.messageBox("Invalid username!")
			endIf
		endIf
	endIf
endEvent

;Displayed when the user hovers over the mods on switch
event OnOptionHighlight(int option)
	if (option == exergameModOnSwitch)
		if (!exergameModOn)
			SetInfoText("Turns off in-game experience and allows you to gain experience from logged workouts.")
		endIf
	elseIf (option == exergameModOffSwitch)
		SetInfoText("Turns off Exergame Mod and allows you to gain experience in game.")
	elseIf (option == forceFetchButton)
		SetInfoText("Check for workouts that were not detected when the game launched.")
	endIf
endEvent

;TODO validate username with backend
bool function validUsername(string username)
	return true
endFunction

function checkLevelUps()
	string workouts
	workouts = fetchWorkouts("Skyrim",playerReference.syncedUserName,Game.getPlayer().getLevel())
	;if(isOldSave(creationDate))
		;showDebugMessage("Old save detected.")
		;workouts = getWorkoutsFromBestWeek(weekNumber)
	;endIf
	int weekNumber
	if(workouts != "")
		if(workouts == "Prior Workout")
			showDebugMessage("Prior workouts detected.")
			doLevelUp(4,3,3)
		else
			string levelUpsString = getLevelUpsAsString(playerReference.outstandingLevel,workouts)

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
			playerReference.outstandingLevel = getOutstandingLevel(levelUpsString)
			updateXpBar(levelUpsString)
		endIf
	else
		showDebugMessage("No workouts found yet.")
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
	showDebugMessage("Congratulations.\nYou have reached level " + currentLevel + "!")
	showDebugMessage("Health (+" + health + ")\nStamina (+" + stamina + ")\nMagicka (+" + magicka + ")")
endFunction

function showDebugMessage(string msg)
	debug.messageBox(msg)
	Utility.waitMenuMode(0.1)
endFunction

;update the xp bar to show the progress gained
function updateXpBar(string levelUpsString)
	float outstandingHealth = getLevelComponent(levelUpsString,0,"H")
	float outstandingStamina = getLevelComponent(levelUpsString,0,"S")
	float outstandingMagicka = getLevelComponent(levelUpsString,0,"M")
	float outstandingWeight = outstandingHealth + outstandingStamina + outstandingMagicka
	Game.setPlayerExperience(outstandingWeight)
endFunction