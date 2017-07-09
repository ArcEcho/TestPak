// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "PlatformFile.h"
#include "PlatformFileManager.h"
#include "IPlatformFilePak.h"
#include "Engine/StreamableManager.h"
#include "PackageName.h"
#include "AssetRegistryModule.h"
#include "TestPak.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"



#define LOCTEXT_NAMESPACE "FHUHU"

class FMyFileVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
    virtual bool Visit(const TCHAR *FilenameOrDirectory, bool bIsDirectory) override
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

void UMyGameInstance::MountPak(const FString &InPakFilename)
{
    //It is not necessary to new a FPakPlatformFile object, because you can mount multiple paks
    // to one pak platform file.
    if (MyPakPlatformFile == nullptr)
    {
        MyPakPlatformFile = new FPakPlatformFile();                                              //The FPlatformFileManager will be responsible for the life time of this.
        MyPakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT("")); //Just use current topmost PlatformFile as the inner lower level PlatformFile of the new PakPlatformFile.
        FPlatformFileManager::Get().SetPlatformFile(*MyPakPlatformFile);                         //Set the new PakPlatformFile to the FPlatformFileManager's topmost PlatformFile. UE4 will use it to read files.
    }

    FString MountPoint = FPaths::GameContentDir();
    FPaths::MakeStandardFilename(MountPoint);
    FString PakFilename = InPakFilename;
    FPaths::MakeStandardFilename(PakFilename);
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

void UMyGameInstance::MountTestSplitedPaks()
{
    FString TargetDir = FPaths::GameDir() + "/SplitedPaks/";
    FMyFileVisitor MyFileVisitor;
    FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*TargetDir, MyFileVisitor);

    for (auto &filename : MyFileVisitor.Files)
    {
        MountPak(filename);
    }
}

void UMyGameInstance::ExecuteTestCode(const UObject *ContextObject)
{
//#define USE_PAK_PRECACHE (!IS_PROGRAM && !WITH_EDITOR)
//
//#if USE_PAK_PRECACHE
//    LogAndPrintToScreen("WTF");
//#endif
//   
//   
//    auto LoadedClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/BP/BP_MyActor.BP_MyActor_C'"));
//    //auto LoadedClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/MyData/BP/BP_TestActor.BP_TestActor_C'"));
//    auto LoadedMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Meshes/SM_Test.SM_Test'"));
//
//    if (LoadedClass)
//    {
//        FVector TargetLocation(0.0f, 0.0f, 300.0f);
//        auto SpawnedActor = GEngine->GetWorldFromContextObject(ContextObject)->SpawnActor(LoadedClass, &TargetLocation);
//        auto TargetMeshComp = Cast<UStaticMeshComponent>(SpawnedActor->GetComponentsByClass(UStaticMeshComponent::StaticClass())[0]);
//        TargetMeshComp->SetStaticMesh(LoadedMesh);
//    }



    auto Message = LOCTEXT("HEHEHE", "HOHOHO");
    FNotificationInfo Info(Message);
    Info.bFireAndForget = true;
    Info.ExpireDuration = 5.0f;
    Info.bUseSuccessFailIcons = false;
    Info.bUseLargeFont = false;
    FSlateNotificationManager::Get().AddNotification(Info);
}

void UMyGameInstance::LogAndPrintToScreen(const FString &Message, const FColor &MessageColor /*= FColor::Purple*/)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, MessageColor, *Message);
}

FStreamableManager &UMyGameInstance::GetStreamableManager()
{
    static FStreamableManager StreamableManager;
    return StreamableManager;
}
