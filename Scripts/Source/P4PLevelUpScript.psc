Scriptname P4PLevelUpScript extends activemagiceffect  

import StringUtil

P4PExergamingMCMPlayerAlias property playerReference auto 


Event OnEffectStart(Actor akTarget, Actor akCaster)
	;the player has activated the potion and is now begining the level up

	string RemainingDists = levelUp()
	SaveRemainingDists(RemainingDists)



	;Debug.MessageBox("An orb of experience has been used to level up. You have gained " + healthAmount + " into health, " + staminaAmount + " into stamina, " + magickaAmount + " into magicka!")
	Debug.Notification("WORKING: LEVEL UP")
EndEvent

string Function levelUp()
	;Bug: If both fiss calls are done at the same time, it is very likely it'll fuck up causing only one of the updates to occur. Threading issue. Not sure how we'll tackle it.

	FISSInterface fread = FISSFactory.getFISS()
	If !fread 
		Debug.MessageBox("Fiss is not installed. Mod will not work correctly")
		return None
	EndIf

	;grab all the level distributions
	fread.beginLoad(playerReference.syncedUserName + playerReference.syncedSaveID + "_StackedLevelDistributions.txt")
	string LevelDistributions = fread.LoadString("Level_Distributions")
	if (IsPunctuation(LevelDistributions))
		Debug.messageBox("it is punctuation")
		LevelDistributions = ""
	EndIf
	string end = fread.endLoad()
	Debug.MessageBox("This is the current level distribution: " + LevelDistributions )
	
	;split the string to get the level dists we need
	string CurrentDist = SubString(LevelDistributions, 0, 11)
	string RemainingDists = SubString(LevelDistributions, 11)
	

	;just changed the implementation of the substring to grab the correct values
	string healthString = SubString(CurrentDist , 1, 2)
	string staminaString = SubString(CurrentDist , 5, 2)
	string magickaString = SubString(CurrentDist , 9, 2)
	
	int healthAmount = healthString as int 
	int staminaAmount = staminaString as int
	int magickaAmount = magickaString as int

	;Need to adjust positions of other levelDists in the file so we dont lose them/reuse the same one over and over
	Actor player = Game.GetPlayer()
	player.ModActorValue("health", healthAmount)
	player.ModActorValue("stamina", staminaAmount)
	player.ModActorValue("magicka", magickaAmount)

	Game.SetPlayerLevel(Game.GetPlayer().GetLevel() + 1)
	Game.SetPerkPoints(Game.GetPerkPoints() + 1)

	Debug.MessageBox("You have drank a Potion of Experience to reach level" + (Game.GetPlayer().GetLevel() + (1 as int)) + ". You have gained " + magickaAmount + " into magicka, " + healthAmount + " into health and " + staminaAmount + " into stamina!")
	return RemainingDists
EndFunction

;Reposition 
Function SaveRemainingDists(string remaingingDists)
	FISSInterface fwrite = FISSFactory.getFISS()
	If !fwrite
		Debug.MessageBox("Fiss is not installed. Mod will not work correctly")
		return
	EndIf

	fwrite.beginSave(playerReference.syncedUserName + playerReference.syncedSaveID + "_StackedLevelDistributions.txt", "P4P")

	Actor player = Game.GetPlayer()
	fwrite.saveString("Level_Distributions", remaingingDists)

	string end = fwrite.endSave()
	;File looks very fucked after a tidy when opened in notepad, however it functions correctly when manipulated by Creation Kit.
EndFunction
