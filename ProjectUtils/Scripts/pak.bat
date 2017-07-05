@echo off
set UnrealPakExePath="C:\Program Files\Epic Games\UE_4.16\Engine\Binaries\Win64\UnrealPak.exe"

set REL_PATH=.
set ABS_PATH=

rem
pushd %REL_PATH%

rem
set ABS_PATH=%CD%

rem
popd

set RootDir=%ABS_PATH%
set OutputPakFile=%RootDir%\Test.pak
set SourceFileList=%RootDir%\paklist.txt

REM %UnrealPakExePath% %OutputPakFile% -create=%SourceFileList% 
%UnrealPakExePath% %OutputPakFile% "C:\Users\zhoumy\Desktop\TestPak\CookedContentFromAnotherProject\Textures\T_Test.*" "C:\Users\zhoumy\Desktop\TestPak\CookedContentFromAnotherProject\dummy.uasset"

