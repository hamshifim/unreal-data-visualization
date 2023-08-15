unreal-data-visualization
=

Quick Project Setup with example data

option 1 if you have the multi project setup.
-
1. Run ./data-wielder/wdata/src/apps/stars/prep_stars.ipynb

Option 2 manual example.
-
1. Copy ./Config/DefaultGameExample.ini to ./Config/DefaultGame.ini
1. Edit file paths according to your setup
1. Copy ./Config/DataConfigStarsExample.json to ./Config/DataConfig.json
1. Edit file paths according to your setup
1. Copy all the files from ./Content/sample_data_stars into ./Content/Data.
1. Copy all the files from ./Content/sample_structs_stars into ./Source/Pepticom4D.


Build
-
1. Build the project.
1. Open unreal editor recent project and browse to ./Source/Pepticom4D.uproject
1. File->OpenLevel->Scroll To MainMap and choose it a dark level should appear.
1. If it's your first run or you have new data and structs Use unreal GUI (or if you know of a better way tell us) to import the data structure and map it to the structs.
   1. Content Drawer -> +Add -> import data -> choose stars_metadata -> associate with StarsMetaDataStruct
1. Run the program using the Green arrow. Wait 1-5 minutes for the data to load.


For Windows Visual Studio
-
Generate Visual Studio project files within Unreal.
Download the Starter Pack within Unreal and rebuild the Visual Studio solution.

