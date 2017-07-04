// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "PlatformFile.h"
#include "PlatformFileManager.h"
#include "IPlatformFilePak.h"
#include "Engine/StreamableManager.h"
#include "PackageName.h"
#include "AssetRegistryModule.h"
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
    //It is not necessary to new a FPakPlatformFile object, because you can mount multiple paks
    // to one pak platform file.
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

void UMyGameInstance::UnmountPak(const FString &PakFilename)
{
    if (MyPakPlatformFile->Unmount(*PakFilename))
    {
        LogAndPrintToScreen(FString::Printf(TEXT("    Succeeded to unmount %s "), *PakFilename), FColor::Green);
    }
    else
    {
        LogAndPrintToScreen(FString::Printf(TEXT("    Failed to mount %s"), *PakFilename), FColor::Red);
        return;
    }

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

FString UMyGameInstance::GetPakRootDir()
{
    FString Filename = FPaths::GameDir();
    return FPaths::ConvertRelativePathToFull(Filename);
}

void UMyGameInstance::GatherReferenceInformation()
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    
    if (AssetRegistryModule.Get().IsLoadingAssets())
    {
        // We are still discovering assets, listen for the completion delegate before building the graph
        //if (!AssetRegistryModule.Get().OnFilesLoaded().IsBoundToObject(this))
        //{
        //    AssetRegistryModule.Get().OnFilesLoaded().AddSP(this, &SReferenceViewer::OnInitialAssetRegistrySearchComplete);
        //}
    }    
    
    UE_LOG(LogTemp, Warning, TEXT("++++++++++++++++++++++++"));

    TArray<FName> D;
    bool bD = AssetRegistryModule.Get().GetDependencies("/Game/Maps/InitialMap", D, EAssetRegistryDependencyType::Hard);
    for (auto &d : D)
    {
        UE_LOG(LogTemp, Warning, TEXT("D ---- %s"), *d.ToString());
    }

    TArray<FName> R;
    bool bR = AssetRegistryModule.Get().GetReferencers("/Game/Maps/InitialMap", R, EAssetRegistryDependencyType::Hard);
    for (auto &r : R)
    {
        UE_LOG(LogTemp, Warning, TEXT("R ---- %s"), *r.ToString());
    }

    UE_LOG(LogTemp, Warning, TEXT("++++++++++++++++++++++++"));

    if (bD)
    {
        UE_LOG(LogTemp, Warning, TEXT("DDDDDDDDDDDDDDDDDDDDD"));
    }

    if (bR)
    {
        UE_LOG(LogTemp, Warning, TEXT("RRRRRRRRRRRRRRRRRRRR"))
    }
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

