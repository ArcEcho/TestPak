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

set RootDir=%ABS_PATH%

set TargetPakFilePart_01=%RootDir%\Paks\Test_split_part_01.pak
set TargetPakFilePart_02=%RootDir%\Paks\Test_split_part_02.pak

echo split part_01:
%UnrealPakExePath% %TargetPakFilePart_01%  -extract -list

echo= 
echo split part_02:
%UnrealPakExePath% %TargetPakFilePart_02%  -extract -list