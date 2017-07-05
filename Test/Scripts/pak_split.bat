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
set OutputPakFilePart_01=%RootDir%\Paks\Test_split_part_01.pak
set OutputPakFilePart_02=%RootDir%\Paks\Test_split_part_02.pak
set OutputPakFilePart_03=%RootDir%\Paks\Test_split_part_03.pak
set OutputPakFilePart_04=%RootDir%\Paks\Test_split_part_04.pak
set OutputPakFilePart_05=%RootDir%\Paks\Test_split_part_05.pak
set SourceFileListPart_01=%RootDir%\Scripts\paklist_part_01.txt
set SourceFileListPart_02=%RootDir%\Scripts\paklist_part_02.txt
set SourceFileListPart_03=%RootDir%\Scripts\paklist_part_03.txt
set SourceFileListPart_04=%RootDir%\Scripts\paklist_part_04.txt
set SourceFileListPart_05=%RootDir%\Scripts\paklist_part_05.txt

%UnrealPakExePath% %OutputPakFilePart_01% -create=%SourceFileListPart_01% -order=CookerOpenOrder.log

%UnrealPakExePath% %OutputPakFilePart_02% -create=%SourceFileListPart_02% -order=CookerOpenOrder.log

%UnrealPakExePath% %OutputPakFilePart_03% -create=%SourceFileListPart_03% -order=CookerOpenOrder.log

%UnrealPakExePath% %OutputPakFilePart_04% -create=%SourceFileListPart_04% -order=CookerOpenOrder.log

%UnrealPakExePath% %OutputPakFilePart_05% -create=%SourceFileListPart_05% -order=CookerOpenOrder.log

