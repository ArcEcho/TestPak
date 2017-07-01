// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "PlatformFile.h"
#include "PlatformFileManager.h"
#include "IPlatformFilePak.h"
#include "Engine/StreamableManager.h"
#include "PackageName.h"
#include "TestPak.h"


class FMyFileVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
    virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
    {
        if (!bIsDirectory)
        {
            Files.Add(FilenameOrDirectory);
        }
        else
        {
            Directories.Add(FilenameOrDirectory);
        }

        return true;
    }

    TArray<FString> Files;
    TArray<FString> Directories;
};



void UMyGameInstance::MountPak(const FString &PakFilename)
{
    if (MyPakPlatformFile == nullptr)
    {
        MyPakPlatformFile = new FPakPlatformFile();                                                 //The FPlatformFileManager will be responsible for the life time of this.
        MyPakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT(""));    //Just use current topmost PlatformFile as the inner lower level PlatformFile of the new PakPlatformFile.
        FPlatformFileManager::Get().SetPlatformFile(*MyPakPlatformFile);                            //Set the new PakPlatformFile to the FPlatformFileManager's topmost PlatformFile. UE4 will use it to read files.
    }

    FString MountPoint = FPaths::GameContentDir();
    if (MyPakPlatformFile->Mount(*PakFilename, 0, *MountPoint))
    {
        LogAndPrintToScreen(FString::Printf(TEXT("    Succeeded to mount %s -at-> %s"), *PakFilename, *MountPoint), FColor::Green);
    }
    else
    {
        LogAndPrintToScreen(FString::Printf(TEXT("    Failed to mount %s at %s"), *PakFilename, *MountPoint), FColor::Red);
        return;
    }

    //Log how many pak files was mounted.
    //TODO

    //Log what files the current pak file provides.
    //TODO 

    //Log all files can be accessed in the game content directory from MyPakPlatformFile node, includes inner lower PlatformFile node recursively.
    //Here MyPakPlatformFile is regarded as the topmost node of the visit chain.
    //
    {
        UE_LOG(LogTemp, Log, TEXT("    Log what can be accessed in the game content dir by the FPlatformFileManager's topmost IPlatforFile:"));
        FMyFileVisitor MyFileVisitor;
        MyPakPlatformFile->IterateDirectoryRecursively(*FPaths::GameContentDir(), MyFileVisitor);
        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("        %s"), *filename);
        }
        UE_LOG(LogTemp, Log, TEXT(" "));
    }
}

void UMyGameInstance::UnmountPak(const FString &PakFilepath)
{
    //TODO
}

void UMyGameInstance::LogAndPrintToScreen(const FString &Message, const FColor &MessageColor /*= FColor::Purple*/)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, MessageColor, *Message);
}

FStreamableManager & UMyGameInstance::GetStreamableManager()
{
    static FStreamableManager  StreamableManager;
    return StreamableManager;
}
