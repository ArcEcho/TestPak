@echo off
set UnrealPakExePath="C:\Program Files\Epic Games\UE_4.16\Engine\Binaries\Win64\UnrealPak.exe"

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

set TargetPakFile=%RootDir%\Paks\Test.pak

%UnrealPakExePath% %TargetPakFile%  -extract -list