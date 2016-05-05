Scriptname ExergameConfigMenu extends ski_configbase  

import PluginScript
import StringUtil

;reference to the playerreference script
P4PExergamingMCMPlayerAlias property playerReference auto

;option IDs
int exergameModOnSwitch
int exergameModOffSwitch
int syncStatus
int checkForWorkoutsButton

;option values
bool exergameModOn = false
bool checkingForWorkouts = false

int count = 0

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
			checkForWorkoutsButton = AddToggleOption("Check for workouts",checkingForWorkouts)
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
			ForcePageReset()
			ShowMessage("Unsync complete.", false, "$Ok")
			Game.requestSave()
		endIf
	elseIf (option == checkForWorkoutsButton)
		;Tell the user this will take time and ask for confirmation
		bool checkForWorkouts = ShowMessage("Are you sure?\nThis will take several minutes to complete.", true, "$Yes", "$No")
		if (checkForWorkouts)
			checkForWorkouts()
		endIf
	endIf
endEvent

;Executes when the user tries to turn the mod on
Event OnOptionInputOpen(int option)
	;show user username entry dislog
	SetInputDialogStartText("Please enter your username...")
EndEvent

;Executes once the user has entered a username
Event OnOptionInputAccept(int option, string userInput)
	if (option == exergameModOnSwitch)
		bool turnOnExergaming = ShowMessage("Are you sure?\nYou will not be able to gain levels in game while Exergame Mod is on.", true,  "$Yes", "$No")
		if (turnOnExergaming == true)
			string userName = userInput
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
		SetInfoText("Turns off in-game experience and allows you to gain experience from logged workouts.")
	elseIf (option == exergameModOnSwitch)
		SetInfoText("Turns off Exergame Mod and allows you to gain experience in game.")
	endIf
endEvent

;TODO validate username with backend
bool function validUsername(string username)
	return true
endFunction

;TODO create a force fetch method in c++
function checkForWorkouts()
	checkingForWorkouts = true
	debug.Notification("Checking for workouts")
endFunction