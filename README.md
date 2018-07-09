## exergaming-mod
This is a mod made to turn Skyrim into an asynchronous exergame. The mod consists of two visual studio projects, scripts written in papyrus, and the .esp file required for the mod to be used.
Creation Kit is needed to edit and compile script files, and edit in game objects. Changes to the .esp can only be done through the Creation Kit.

## Editing the C++ executables
Visual studio is required for editing C++ and to build the required .dll files.
Note: The generated files are the ones required. These are plugin_example.dll, webserviceTest.exe, and cpprest140_2_7.dll

## Editing Papyrus files
Sublime text can be used to edit and compile scripts using [this](https://github.com/Kapiainen/SublimePapyrus) sublime package.
After making changes to papyrus .psc files, the scripts can be compiled in Sublime and should replace any old .pex scripts of the same name.

## Editing the Creation Kit files
If you want to objects from the Creation Kit, open the Creation Kit and set the Exergaming.esp as the active file. Filtering by "Exergaming" should list the objects used by the exergaming mod. To change a message for example select that message object, change the message text and then click ok. After making changes, save and then select File->Export->Messages. This method should apply to all objects but the exporting method will change as the object type does.

## Required mods
This mod is dependant on two other Skyrim mods.
[SKSE](http://skse.silverlock.org/) is required for C++ to be run in skyrim.
[SkyUI](http://www.nexusmods.com/skyrim/mods/3863/) is required for the in game menu.

## File Structure
Exergaming.esp, the Interface folder and the Scripts folder should all be placed in the Skyrim/Data folder. This puts them in the required file heirarchy to use the mod.
Papyrus script files should be placed in the Skyrim/Data/Scripts/Source folder, this will ensure that the generated .pex files will be Skyrim/Data/Scripts where they shuld be.
The required .dll and .exe files from the visual studio projects need to be put into Skyrim/SKSE/Plugins folder.
