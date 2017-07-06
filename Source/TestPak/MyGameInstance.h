// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


class FPakPlatformFile;
struct FStreamableManager;

/**
 * 
 */
UCLASS()
class TESTPAK_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintCallable, Category = TestPak)
    void MountPak(const FString &PakFilename);

    UFUNCTION(BlueprintCallable, Category = TestPak)
    void UnmountPak(const FString &PakFilename);

    UFUNCTION(BlueprintCallable, Category = TestPak)
        FString GetPakRootDir();

    UFUNCTION(BlueprintCallable, Category = TestPak)
        void MountTestSplitedPaks();

protected:
    void LogAndPrintToScreen(const FString &Message, const FColor &MessageColor = FColor::Purple);
 
protected:
 
    FPakPlatformFile* MyPakPlatformFile;

    FStreamableManager &GetStreamableManager();

};
