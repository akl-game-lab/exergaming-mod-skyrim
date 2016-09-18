## exergaming-mod
This is a mod made to turn Skyrim into an asynchronous exergame. The mod consists of two visual studio projects, scripts written in papyrus, and the .esp file required for the mod to be used.

## Editing
Visual studio is required for editing C++ and to build the required .dll files.
Note: The generated files are the ones reuired. These are plugin_example.dll, webserviceTest.exe, and cpprest140_2_7.dll
Creation Kit is needed to edit and compile script files, and edit in game objects. Changes to the .esp can only be done through the Creation Kit
Sublime text can be used to edit and compile scripts using [this](https://github.com/Kapiainen/SublimePapyrus) sublime package

## Required mods
This mod is dependant on two other Skyrim mods.
[SKSE](http://skse.silverlock.org/) is required for C++ to be run in skyrim.
[SkyUI](http://www.nexusmods.com/skyrim/mods/3863/) is required for the in game menu.

## File Structure
Exergaming.esp, the Interface folder and the Scripts folder should all be placed in the Skyrim/Data folder. This puts them in the required file heirarchy to use the mod.
Papyrus script files should be placed in the Skyrim/Data/Scripts/Source folder, this will ensure that the generated .pex files will be Skyrim/Data/Scripts where they shuld be.
The required .dll and .exe files from the visual studio projects need to be put into Skyrim/SKSE/Plugins folder.
