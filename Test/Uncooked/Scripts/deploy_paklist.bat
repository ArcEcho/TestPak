@echo off
set REL_PATH=..
set ABS_PATH=

rem
pushd %REL_PATH%

rem
set ABS_PATH=%CD%

rem
popd

echo Relative path: %REL_PATH%
echo Maps to path: %ABS_PATH%

set RootDir=%ABS_PATH%
set TragetDir=%RootDir%\assets\*.*
set SourceFileList=%RootDir%\Scripts\paklist.txt

echo %TragetDir% >%SourceFileList%
