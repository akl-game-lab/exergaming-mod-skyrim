Scriptname ExergameConfigMenu extends ski_configbase  

import PluginScript
import StringUtil

;reference to the playerReference script
P4PExergamingMCMPlayerAlias property playerReference auto

;option IDs
int exergameModOnSwitch
int exergameModOffSwitch
int syncStatus
int forceFetchButton
int forceFetchCancelButton
int diagnosisButton
int submitButton

int ownStatsButton
int magickaSlider
int healthSlider
int staminaSlider
int magickaPoints
int healthPoints
int staminaPoints
int submitpoints

;option values
bool exergameModOn = false
bool forceFetch = false
bool forceFetchCancel = false
bool runDiagnosis = false

;Event log file
String eventLog = "SkyrimExergameMod_EventLog"

;Defines the number of pages in the MCM
event OnConfigInit()
	Pages = new string[2]
	Pages[0] = "Settings"
	Pages[1] = "Attributes"
endEvent

; SCRIPT VERSION
int function GetVersion()
	return 2
endFunction

event OnVersionUpdate(int a_version)
	; a_version is the new version, CurrentVersion is the old version
	if (a_version >= 2 && CurrentVersion < 2)
		Pages = new string[2]
		Pages[0] = "Settings"
		Pages[1] = "Attributes"
	endIf
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
			string shortenedUsername = getShortenedUsername(syncedUserName)
			syncStatus = AddTextOption("Currently synced with ", shortenedUsername)
			if (playerReference.forceFetchMade == true);Menu options shown when a force fetch is being made
				AddTextOption("Close the menu while checking for workouts.","");
				forceFetchCancelButton = AddToggleOption("Cancel",forceFetchCancel)
			else
				if(playerReference.oldSaveLoaded == false)
					forceFetchButton = AddToggleOption("Check for recent workouts",forceFetch)
				endIf
				exergameModOffSwitch = AddToggleOption("Mod Active", exergameModOn)
			diagnosisButton = AddToggleOption("Run Diagnostics", runDiagnosis)
			endIf
		else
			exergameModOnSwitch = AddInputOption("Please enter your email", "");runs code in OnOptionInputOpen()
		endIf
	elseif(page == "Attributes")
		UnloadCustomContent()
		;initial options setup
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)
		AddHeaderOption("Exergame Mod")
		ownStatsButton = AddToggleOption("Manage My Own Attributes", playerReference.manageOwnStats)
		if (playerReference.manageOwnStats)
			magickaSlider = AddSliderOption("	Magicka", playerReference.magickaPoints, "{0}")
			healthSlider = AddSliderOption("	Health", playerReference.healthPoints, "{0}")
			staminaSlider = AddSliderOption("	Stamina", playerReference.staminaPoints, "{0}")
			submitButton = AddToggleOption("Save", submitpoints)
		endif
	endIf
endEvent

;Executes when the user opens a slider
event OnOptionSliderOpen(int option)
	if(option == healthSlider)
		SetSliderDialogStartValue(0)
		SetSliderDialogDefaultValue(playerReference.healthPoints)
		SetSliderDialogRange(0, 10)
		SetSliderDialogInterval(1)
	elseif(option == magickaSlider)
		SetSliderDialogStartValue(0)
		SetSliderDialogDefaultValue(playerReference.magickaPoints)
		SetSliderDialogRange(0, 10)
		SetSliderDialogInterval(1)
	elseif(option == staminaSlider)
		SetSliderDialogStartValue(0)
		SetSliderDialogDefaultValue(playerReference.staminaPoints)
		SetSliderDialogRange(0, 10)
		SetSliderDialogInterval(1)
	endif
endEvent

;Executes when the user confirms a selection on a slider. 
event OnOptionSliderAccept(int option, float value)
	if(option == healthSlider)
		healthPoints = value as int
		setSliderOptionValue(healthSlider, healthPoints , "{0}")
	elseif(option == magickaSlider)
		magickaPoints = value as int
		setSliderOptionValue(magickaSlider, magickaPoints, "{0}")
	elseif(option == staminaSlider)
		staminaPoints = value as int
		setSliderOptionValue(staminaSlider, staminaPoints, "{0}")
	endif
endEvent 

;Executes when the user selects an option in the menu
event OnOptionSelect(int option)
	if (option == diagnosisButton)
		showMessage("running diagnostics")
		runDiagnosis(playerReference.syncedUserName)
		showMessage("diagnosis finished, please see Debug.txt")
	elseif(option == ownStatsButton)
		if(!playerReference.manageOwnStats)
			bool managestats = ShowMessage("Manage your own attributes?", true, "$Yes", "$No")
			if(managestats)
				playerReference.manageOwnStats = true
			endif
		else
			bool managestats = ShowMessage("Let the mod assign your attributes automatically>", true, "$Yes", "$No")
			if(managestats)
				playerReference.manageOwnStats = false
			endIf
		endif
	elseif(option == submitButton)
		if(healthPoints + magickaPoints + staminaPoints != 10)
			bool submit = ShowMessage("Attribute points must add up to 10", false)
		else
			playerReference.healthPoints = healthPoints
			playerReference.magickaPoints = magickaPoints
			playerReference.staminaPoints = staminaPoints
			bool submit = ShowMessage("The following attributes will be assigned on level up: \nMagicka: "+ playerReference.magickaPoints as string + "\nHealth: " + playerReference.healthPoints as string + "\nStamina: " + playerReference.staminaPoints as string, false)
		endif
	elseif (playerReference.forceFetchMade == false)
		if (option == exergameModOffSwitch)
			;Show the confirmation to turn off Exergaming mode
			bool turnOffExergaming = ShowMessage("Are you sure?\nThis will turn off Exergame Mod in the current save.", true, "$Yes", "$No")
			if (turnOffExergaming)
				exergameModOn = false
				playerReference.uninitialise()
				ShowMessage("Unsync complete.", false, "Ok")
				playerReference.saveRequested = true
			endIf
		elseIf (option == forceFetchButton);Start force fetch
			
			Debug.TraceUser(eventLog, "001 Force Fetch Started", 0)

			int serverResponse = startForceFetch("Skyrim",playerReference.syncedUserName)
			
			Debug.TraceUser(eventLog, "002 Server Returned: " +serverResponse, 0)
			
			if( serverResponse == 200 )
				playerReference.forceFetchMade = true
				playerReference.pollStartTime = currentDate()
			elseIf( serverResponse == 404 )
				ShowMessage("INVALID STATE ERROR\n\nPlease contact exergaming customer support with the current date and time.", false)
			else
				ShowMessage("SERVER ERROR\n\nPlease try again in a few minutes.\n\nIf this error persists, please contact exergaming customer support with the current date and time.", false)
			endIf
		endIf
	elseIf (option == forceFetchCancelButton)
		playerReference.forceFetchMade = false
		
		Debug.TraceUser(eventLog, "003 Force Fetch Cancelled", 0)
	endIf
	ForcePageReset()
endEvent

;Executes when the user tries to turn the mod on
Event OnOptionInputOpen(int option)
	;ShowMessage("You will not be able to gain levels in game while Exergame Mod is on.", false)
	;show user username entry dialog
	;SetInputDialogStartText("Please enter your exergaming email...")
EndEvent

;Executes once the user has entered a username
Event OnOptionInputAccept(int option, string userInput)
	if (option == exergameModOnSwitch)
		string username = userInput
		if(validUsername("Skyrim",username))	
			playerReference.syncedUserName = username
			;Set exp variables to 0, to turn off in-game experience
			Game.SetPlayerExperience(0)
			Game.SetGameSettingFloat("fXPPerSkillRank", 0)
			exergameModOn = true
			ForcePageReset()
			string msg = "Sync with " + username + " complete."
			playerReference.initialise()
			ShowMessage(msg, false, "Ok")
			playerReference.startNormalFetchWithErrorHandling()
			
			Debug.TraceUser(eventLog, "004User synced with: " +username, 0)
			
		else
			ShowMessage("Invalid email!", false)
			
			Debug.TraceUser(eventLog, "005 User attempted to sync with: " +username, 0)
			Debug.TraceUser(eventLog, "006 Sync failed, invalid email", 1)
			
		endIf
	endIf
endEvent

;Displayed when the user hovers over the mods on switch
event OnOptionHighlight(int option)
	if (playerReference.forceFetchMade == true)
		if (option == forceFetchCancelButton)
			SetInfoText("Stop checking for recent workouts.")
		endIf
	else
		if (option == exergameModOnSwitch)
			if (!exergameModOn)
				SetInfoText("Turns off in-game experience and allows you to gain experience from logged workouts.")
			endIf
		elseIf (option == exergameModOffSwitch)
			SetInfoText("Turns off Exergame Mod and allows you to gain experience in game.")
		elseIf (option == forceFetchButton)
			SetInfoText("Check for workouts that were not detected when the game launched.")
		endIf
	endIf
endEvent