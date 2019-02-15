![ObjectARX2019](https://img.shields.io/badge/ObjectARX-2019-brightgreen.svg)
![BuildPassing](https://img.shields.io/badge/build-passing-brightgreen.svg)
![AutoCAD2019](https://img.shields.io/badge/AutoCAD-2019-blue.svg)

## How to handle references to other entities
An example of handling some common operations for certain types of entities. This does not attempt to be a full-featured entity in the sense that it fills out every single function that it is supposed to. 
Its main  purpose is to illustrate how to deal with pointing to a dictionary-resident"Style" object 
This sample is stripped out from [Jim Awe's](https://www.linkedin.com/in/jim-awe-4630a94/) classic ArxDBG, this mainly focus on establishing hard linkage to other entities, dealing copy with in and across database.

## Working Demo

![WorkingDemo](https://github.com/MadhukarMoogala/adn_adskLogo/blob/master/media/demo.gif)

## Prerequisite
- ObjectARX 2019 SDK
- Visual Studio 2017 Professional or Community
- AutoCAD 2019

## Building Sample
- `git clone https://github.com/MadhukarMoogala/adn_adskLogo.git`
- `cd adn_adskLogo\adskLogo`
-  edit `Autodesk.arx-2019.props`, find `$(ArxSdkDir)` replace with your SDK path
-  launch visual developer command prompt `devenv adskLogo.sln`
- `msbuild AdskLogo.sln /t:Build /p:Configuration=Debug;Platform=x64`

## License

This sample is licensed under the terms of the [MIT License](http://opensource.org/licenses/MIT).
Please see the [LICENSE](LICENSE) file for full details.

## Written by

Madhukar Moogala (Forge Partner Development), 
http://forge.autodesk.com

