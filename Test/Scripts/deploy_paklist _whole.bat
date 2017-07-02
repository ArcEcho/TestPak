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
set TargetDir=%RootDir%\CookedContentFromAnotherProject\*.*
set SourceFileList=%RootDir%\Test\Scripts\paklist_whole.txt

echo %TargetDir% >%SourceFileList%
