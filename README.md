# TestPak
UE4 Pak Test

### For what
+ little project to help me understanding UE4 working flow of pak.
+ provding some brief code to show how to mount pak files and the load resource from it.

### test what
Pak working flow

### Requires
+ UE4 4.16.2: Code is simple and this version is not prerequisite.
+ Python 3.x: The scripts in ProjectUtils are written with python 3.x.

### Steps

+ Use the ExportAssetDependenciesPlugin to export dependecies infomation of what you want to collect into pak files.
+ Cook all content.
+ Setup the config in ProjectUtils.
+ Use generate bat tool in ProjectUtils to produce pak files. Pay attention to the command line, especially the response files' content.
+ Exclude the assets you have exported above and package your project.
+ Run game and input the root dir's TestPaks path and open MyTestLevel to see the result. 

### Adition
1.Add a plugin to export asset dependencies.

### Bug
+ I do not know why UE4.15.3 just produces uasset file. This will cause unexpected banding effects if you use static ligting.