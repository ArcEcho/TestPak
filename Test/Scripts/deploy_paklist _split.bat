@echo off
set REL_PATH=..\..
set ABS_PATH= 

rem
pushd %REL_PATH%

rem
set ABS_PATH=%CD%

rem
popd

set RootDir=%ABS_PATH%

set SourceFileListPart_01=%RootDir%\Test\Scripts\paklist_part_01.txt
set SourceFileListPart_02=%RootDir%\Test\Scripts\paklist_part_02.txt

echo %RootDir%\CookedContentFromAnotherProject\Maps\*.* >%SourceFileListPart_01%

echo %RootDir%\CookedContentFromAnotherProject\BP\*.* >%SourceFileListPart_02%
echo %RootDir%\CookedContentFromAnotherProject\Materials\*.* >> %SourceFileListPart_02%
echo %RootDir%\CookedContentFromAnotherProject\Meshes\*.* >> %SourceFileListPart_02%
echo %RootDir%\CookedContentFromAnotherProject\Textures\*.* >> %SourceFileListPart_02%

