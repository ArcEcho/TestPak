// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyBlueprintFunctionLibrary.h"

#include "PlatformFile.h"
#include "IPlatformFilePak.h"
#include "PlatformFileManager.h"
#include "Kismet/GameplayStatics.h"
#include "ModuleManager.h"

DEFINE_LOG_CATEGORY(LogMyBPLibrary)

class FMyFileVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
    virtual bool Visit(const TCHAR *FilenameOrDirectory, bool bIsDirectory) override
    {
        FString StandardizedFilenameOrDirectory(FilenameOrDirectory);
        FPaths::MakeStandardFilename(StandardizedFilenameOrDirectory);
        if (!bIsDirectory)
        {
            Files.Add(StandardizedFilenameOrDirectory);
        }
        else
        {
            Directories.Add(StandardizedFilenameOrDirectory);
        }

        return true;
    }

    TArray<FString> Files;
    TArray<FString> Directories;
};

void UMyBlueprintFunctionLibrary::ExecuteTestCode(const FString &InPaksDir)
{
    FString PaksDir = InPaksDir;
    FPaths::MakeStandardFilename(PaksDir);
    UE_LOG(LogMyBPLibrary, Log, TEXT("ExecuteTestCode InPaksDir: %s"), *PaksDir);

    {
        FMyFileVisitor MyFileVisitor;
        FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*PaksDir, MyFileVisitor);

        for (auto &f : MyFileVisitor.Files)
        {
            if (FPaths::GetExtension(f) == "pak")
            {
                UE_LOG(LogMyBPLibrary, Log, TEXT("Find and mount pak file: %s"), *f);
                GetPakPlatformFile()->Mount(*f, 0);
            }
        }
    }
}

void UMyBlueprintFunctionLibrary::BeginDestroy()
{
    Super::BeginDestroy();
    UE_LOG(LogMyBPLibrary, Log, TEXT("UMyBlueprintFunctionLibrary destoryed"));

    delete PakPlatformFile;
}

FPakPlatformFile * UMyBlueprintFunctionLibrary::PakPlatformFile = nullptr;

FPakPlatformFile * UMyBlueprintFunctionLibrary::GetPakPlatformFile()
{
    if (PakPlatformFile == nullptr)
    {
        PakPlatformFile = new FPakPlatformFile();
        PakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT(""));
        FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
    }

    return PakPlatformFile;
}


