// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyBPLibrary, Log, All)

class FPakPlatformFile;

/**
 *
 */
UCLASS()
class MYPROJECT_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, Category = "TestCode")
    static void ExecuteTestCode(const FString &InPaksDir);

public:
    virtual void BeginDestroy()  override;

private:
    static FPakPlatformFile *GetPakPlatformFile();

    static FPakPlatformFile *PakPlatformFile;

};
