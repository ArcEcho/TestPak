// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"

#include "PlatformFile.h"
#include "PlatformFileManager.h"
#include "IPlatformFilePak.h"
#include "Engine/StreamableManager.h"
#include "PackageName.h"

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

    PlayerInputComponent->BindAction("TestA", IE_Pressed, this, &AMyPawn::LoadPakWithUnCookedAsset);
    PlayerInputComponent->BindAction("TestB", IE_Pressed, this, &AMyPawn::LoadPakWtihCookedAsset);
    PlayerInputComponent->BindAction("TestC", IE_Pressed, this, &AMyPawn::TryLoadUnCookedAsset);
    PlayerInputComponent->BindAction("TestD", IE_Pressed, this, &AMyPawn::TryLoadCookedAsset);
    //PlayerInputComponent->BindAction("TestE", IE_Pressed, this, &AMyPawn::HandleTestBActionInput);
}

void AMyPawn::LoadPakWithUnCookedAsset()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("LoadPakWithUnCookedAsset"));

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPakPlatformFile* PakPlatformFile = new FPakPlatformFile();
    PakPlatformFile->Initialize(&PlatformFile, TEXT(""));
    FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);

    FString PakFilename = FPaths::GameDir() + TEXT("Test/Uncooked/Paks/Test.pak");
    FString mountPoint(*(FPaths::GameContentDir() + TEXT("TestPaks/Uncooked/")));

    if (!PakPlatformFile->Mount(*PakFilename, 0, *mountPoint))
    {
        UE_LOG(LogTemp, Log, TEXT("Pak Mount Failed"));
        return;
    }

    {
        FMyFileVisitor MyFileVisitor;
        PlatformFile.IterateDirectoryRecursively(*FPaths::GameContentDir(), MyFileVisitor);

        UE_LOG(LogTemp, Log, TEXT("Iterate PlatformFile Begin"));
        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("File         %s"), *filename);
        }

        for (auto &DirectoryName : MyFileVisitor.Directories)
        {
            UE_LOG(LogTemp, Log, TEXT("Directory    %s"), *DirectoryName);
        }
        UE_LOG(LogTemp, Log, TEXT("Iterate PlatformFile End"));
    }

    {
        FMyFileVisitor MyFileVisitor;
        PakPlatformFile->IterateDirectoryRecursively(*FPaths::GameContentDir(), MyFileVisitor);

        UE_LOG(LogTemp, Log, TEXT("Iterate PakPlatformFile Begin"));
        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("File         %s"), *filename);
        }

        for (auto &DirectoryName : MyFileVisitor.Directories)
        {
            UE_LOG(LogTemp, Log, TEXT("Directory    %s"), *DirectoryName);
        }
        UE_LOG(LogTemp, Log, TEXT("Iterate PakPlatformFile End"));
    }
}

void AMyPawn::LoadPakWtihCookedAsset()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("LoadPakWtihCookedAsset"));

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPakPlatformFile* PakPlatformFile = new FPakPlatformFile();
    PakPlatformFile->Initialize(&PlatformFile, TEXT(""));
    FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);

    FString PakFilename = FPaths::GameDir() + TEXT("Test/Cooked/Paks/Test.pak");
    FString mountPoint(*(FPaths::GameContentDir() + TEXT("TestPaks/Cooked/")));

    if (!PakPlatformFile->Mount(*PakFilename, 0, *mountPoint))
    {
        UE_LOG(LogTemp, Log, TEXT("Pak Mount Failed"));
        return;
    }

    {
        FMyFileVisitor MyFileVisitor;
        PlatformFile.IterateDirectoryRecursively(*FPaths::GameContentDir(), MyFileVisitor);

        UE_LOG(LogTemp, Log, TEXT("Iterate PlatformFile Begin"));
        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("File         %s"), *filename);
        }

        for (auto &DirectoryName : MyFileVisitor.Directories)
        {
            UE_LOG(LogTemp, Log, TEXT("Directory    %s"), *DirectoryName);
        }
        UE_LOG(LogTemp, Log, TEXT("Iterate PlatformFile End"));
    }

    {
        FMyFileVisitor MyFileVisitor;
        PakPlatformFile->IterateDirectoryRecursively(*FPaths::GameContentDir(), MyFileVisitor);

        UE_LOG(LogTemp, Log, TEXT("Iterate PakPlatformFile Begin"));
        for (auto &filename : MyFileVisitor.Files)
        {
            UE_LOG(LogTemp, Log, TEXT("File         %s"), *filename);
        }

        for (auto &DirectoryName : MyFileVisitor.Directories)
        {
            UE_LOG(LogTemp, Log, TEXT("Directory    %s"), *DirectoryName);
        }
        UE_LOG(LogTemp, Log, TEXT("Iterate PakPlatformFile End"));
    }
}


void AMyPawn::TryLoadUnCookedAsset()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TryLoadUnCookedAsset"));
    UE_LOG(LogTemp, Log, TEXT("TryLoadUnCookedAsset"));
    FStreamableManager  StreamableManager;
    //FStringAssetReference AssetReference("/Game/TestPaks/Uncooked/TestMat.TestMat");
    FStringAssetReference AssetReference("/Game/TestPaks/TestMat.TestMat");
    UMaterialInterface *LoadedMaterial = StreamableManager.LoadSynchronous<UMaterialInterface>(AssetReference);
    if (LoadedMaterial)
    {
        UE_LOG(LogTemp, Log, TEXT("Load Succeeded"));
        Mesh->SetMaterial(0, LoadedMaterial);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Load Failed"));
    }


}

void AMyPawn::TryLoadCookedAsset()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("TryLoadCookedAsset"));
    UE_LOG(LogTemp, Log, TEXT("TryLoadCookedAsset"));

    /* FStreamableManager  StreamableManager;
     FStringAssetReference AssetReference("/Game/TestPaks/Cooked/TestMat.TestMat");
     UObject *LoadedObject = StreamableManager.LoadSynchronous(AssetReference);
     if (LoadedObject)
     {
         UE_LOG(LogTemp, Log, TEXT("Load Succeeded"));
     }
     else
     {
         UE_LOG(LogTemp, Log, TEXT("Load Failed"));
     }
 */

    auto &PlatformFileInterface = FPlatformFileManager::Get().GetPlatformFile();
    auto FileHandle = PlatformFileInterface.OpenRead(*(FPaths::GameContentDir() + TEXT("TestPaks/Cooked/test.txt")));

    if (FileHandle)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Read txt Succeeded"));

        int32 MyInteger{ -1 };
        auto bSucceeded = FileHandle->Read(reinterpret_cast<uint8*>(&MyInteger), sizeof(int32));
        if (!bSucceeded)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Failed to read"));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Read integer is %d"), MyInteger));
        }

        delete FileHandle;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Read txt failed"));

    }
}
