// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"

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


// Sets default values
AMyPawn::AMyPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    //TODO
    FPackageName::OnContentPathMounted().AddLambda([](const FString& AssetPath, const FString& ContentPath) { UE_LOG(LogTemp, Log, TEXT("OnContentPathMounted  %s %s"), *AssetPath, *ContentPath); });
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("TestA", IE_Pressed, this, &AMyPawn::LoadPak);
    PlayerInputComponent->BindAction("TestB", IE_Pressed, this, &AMyPawn::TryLoadPackageFromPak);
}

void AMyPawn::LoadPak()
{
    LogAndPrintToScreen("LoadPak-----------------------------------------");;

    FPakPlatformFile* PakPlatformFile = new FPakPlatformFile();                              //The FPlatformFileManager will be responsible for the life time of this.
    PakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT(""));   //Just use current topmost PlatformFile as the inner lower level PlatformFile of the new PakPlatformFile.
    FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);                           //Set the new PakPlatformFile to the FPlatformFileManager's topmost PlatformFile. UE4 will use it to read files.

    FString MountPoint = FPaths::GameContentDir() + "Paks/";

    {
        FString PakFilename = FPaths::GameDir() + TEXT("Test/Cooked/Paks/TestA.pak");


        if (PakPlatformFile->Mount(*PakFilename, 0, *MountPoint))
        {
            LogAndPrintToScreen(FString::Printf(TEXT("    Succeeded to mount %s at %s"), *PakFilename, *MountPoint), FColor::Green);
        }
        else
        {
            LogAndPrintToScreen(FString::Printf(TEXT("    Failed to mount %s at %s"), *PakFilename, *MountPoint), FColor::Red);
            return;
        }
    }


    {
        FString PakFilename = FPaths::GameDir() + TEXT("Test/Cooked/Paks/TestB.pak");


        if (PakPlatformFile->Mount(*PakFilename, 0, *MountPoint))
        {
            LogAndPrintToScreen(FString::Printf(TEXT("    Succeeded to mount %s at %s"), *PakFilename, *MountPoint), FColor::Green);
        }
        else
        {
            LogAndPrintToScreen(FString::Printf(TEXT("    Failed to mount %s at %s"), *PakFilename, *MountPoint), FColor::Red);
            return;
        }
    }

    //{
    //    FString PakFilename = FPaths::GameDir() + TEXT("Test/Cooked/Paks/Test.pak");


    //    if (PakPlatformFile->Mount(*PakFilename, 0, *MountPoint))
    //    {
    //        LogAndPrintToScreen(FString::Printf(TEXT("    Succeeded to mount %s at %s"), *PakFilename, *MountPoint), FColor::Green);
    //    }
    //    else
    //    {
    //        LogAndPrintToScreen(FString::Printf(TEXT("    Failed to mount %s at %s"), *PakFilename, *MountPoint), FColor::Red);
    //        return;
    //    }
    //}

    //Log what have been mounted by the pak file.
    {
        UE_LOG(LogTemp, Log, TEXT("    Log what have been mounted by the pak file:"));
        FMyFileVisitor MyFileVisitor;
        FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*MountPoint, MyFileVisitor);

        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("        %s"), *filename);
        }

        UE_LOG(LogTemp, Log, TEXT(" "));
    }

    //Log what can be accessed from the FPlatformFileManager's topmost IPlatforFile.
    {
        UE_LOG(LogTemp, Log, TEXT("    Log what can be accessed in the game content dir by the FPlatformFileManager's topmost IPlatforFile:"));
        FMyFileVisitor MyFileVisitor;
        FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FPaths::GameContentDir(), MyFileVisitor);

        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("        %s"), *filename);
        }

        UE_LOG(LogTemp, Log, TEXT(" "));
    }

    LogAndPrintToScreen("LoadPak-----------------------------------------");;
}

void AMyPawn::TryLoadPackageFromPak()
{
    LogAndPrintToScreen("TryLoadPackageFromPak-----------------------------------------");

    //Try to read txt file content
    {
        auto FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead(*(FPaths::GameContentDir() + TEXT("Paks/test.txt")));
        int32 MyInteger{ -1 };
        bool bSucceeded = false;
        if (FileHandle)
        {
            bSucceeded = FileHandle->Read(reinterpret_cast<uint8*>(&MyInteger), sizeof(int32));
            delete FileHandle;
        }

        if (!FileHandle)
        {
            LogAndPrintToScreen("    Failed to open txt file in pak", FColor::Red);
        }
        else
        {
            if (bSucceeded)
            {
                LogAndPrintToScreen(FString::Printf(TEXT("    Succeeded to read txt file content, and the read integer is %d"), MyInteger), FColor::Green);
            }
            else
            {
                LogAndPrintToScreen("    Succeeded to open txt file in pak, but failed to read the file content", FColor::Red);
            }
        }
    }

    //Try to load static mesh
    auto LoadedMesh = Cast<UStaticMesh>( TryLoadAsset("SM_Test"));
    if (LoadedMesh)
    {
        Mesh->SetStaticMesh(LoadedMesh);
    }

    //Try to load texture
    TryLoadAsset("T_Test");

    //Try to load material
    TryLoadAsset("M_Test");

    LogAndPrintToScreen("TryLoadPackageFromPak-----------------------------------------");;
}

void AMyPawn::LogAndPrintToScreen(const FString &Message, const FColor &MessageColor /*= FColor::Purple*/)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, MessageColor, *Message);
}

FStreamableManager & AMyPawn::GetStreamableManager()
{
    static FStreamableManager  StreamableManager;
    return StreamableManager;
}

UObject * AMyPawn::TryLoadAsset(const FString &AssetShortName)
{
    FString AssetName("/Game/Paks/");
    AssetName += AssetShortName + "." + AssetShortName;

    FStringAssetReference AssetReference(AssetName);
    auto Result = AssetReference.TryLoad();

    if (Result)
    {
        LogAndPrintToScreen(FString::Printf(TEXT("    Load %s Succeeded"), *AssetName), FColor::Green);
    }
    else
    {
        LogAndPrintToScreen(FString::Printf(TEXT("    Load %s Failed"), *AssetName), FColor::Red);
    }

    return  Result;
}
