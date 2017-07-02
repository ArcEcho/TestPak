# TestPak
UE4 Pak Test

### For what
+ little project to help me understanding UE4 working flow of pak.
+ provding some brief code to show how to mount pak files and the load resource from it.

### test what
I just made all the assets in another project. And use two way to generate pak files. One way is to package them as a whole, and the other way is to package them into two pak files. Mount them and try to open the level in the pak files.

### Requires
+ UE4 4.16.2: Code is simple and this version is not prerequisite.
+ Python 3.x: The scripts in ProjectUtils are written with python 3.x.

### Steps
+ Run deploy bat to create pak response files, these files contain what be packaged into pak files.This step is to discover and write absolute path into corresponding files. 
+ Run pak bat to package files.
+ (Optional) Yon can run pak_extract bat to see what was packaged and how they ary organized.
+ Packaging TestPak project, and copy all the files from ./paks to your packaging destination ./dest/TestPak/MyPaks
+ Run game and test. 