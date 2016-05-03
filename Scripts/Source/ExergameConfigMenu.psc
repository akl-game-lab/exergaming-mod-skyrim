Scriptname ExergameConfigMenu extends ski_configbase  

import PluginScript
import StringUtil

;reference to the playerreference script
P4PExergamingMCMPlayerAlias property playerReference auto

;option IDs
int exergameModOnSwitch
int exergameModOffSwitch

;option values
bool exergameModOn = false

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
			AddTextOption("Currently synced with ", syncedUserName)
			exergameModOffSwitch = AddToggleOption("Turn Exergame Mode off", exergameModOn);runs code in OnOptionSelect()
		else
			exergameModOnSwitch = AddInputOption("Turn Exergame Mod on", "");runs code in OnOptionInputOpen();
		endIf
	endIf
endEvent

;Handles toggling of exergaming mode.
event OnOptionSelect(int option)
	if (option == exergameModOffSwitch)
		if(exergameModOn)
			;Show the confirmation to turn off Exergaming mode (need to change this message prompt to use the implementation for a yes no message from the mcm github)
			bool turnOffExergaming = ShowMessage("Are you sure?\nThis will turn off Exergame Mod in the current save.", true, "$Yes", "$No")
			if (turnOffExergaming)
				exergameModOn = false
				playerReference.syncedUserName = ""

				;reset the exp variables
				Game.SetGameSettingFloat("fXPLevelUpBase", 75)
				Game.SetGameSettingFloat("fXPLevelUpMult", 25)
				Game.SetPlayerExperience(0)
				Game.SetGameSettingFloat("fXPPerSkillRank", 1)
				ForcePageReset()
				game.requestsave()
			endIf
		endIf
	endIf
endEvent

;Executes when the user tries to turn the mod on
Event OnOptionInputOpen(int option)
	;show user username entry dislog
	SetInputDialogStartText("Please enter your username...")
	;runs OnOptionInputAccept()
EndEvent

;Executes once the user has entered a username
Event OnOptionInputAccept(int option, string userInput)
	if (option == exergameModOnSwitch)
		bool turnOnExergaming = ShowMessage("Are you sure?\nYou will not be able to gain levels in game while Exergame Mod is on.", true,  "$Yes", "$No")
		if (turnOnExergaming == true)
			string userName = userInput
			if(validUsername(username))
				playerReference.syncedUserName = username
				debug.messageBox("Sync with " + username + " Complete!")
				Game.SetPlayerExperience(0)
				Game.SetGameSettingFloat("fXPPerSkillRank", 0)
				exergameModOn = true
				ForcePageReset()
				game.requestsave()
			else
				debug.messageBox("Invalid username!")
			endIf
		endIf
	endIf
endEvent

;Displayed when the user hovers over the mods on switch
event OnOptionHighlight(int option)
	if (option == exergameModOffSwitch)
		SetInfoText("Turns off in-game experience and allows you to gain experience from logged workouts")
	endIf
endEvent

;TODO validate username with backend
bool Function validUsername(string username)
	return true
endFunction
