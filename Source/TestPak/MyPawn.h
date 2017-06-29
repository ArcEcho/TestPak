// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS()
class TESTPAK_API AMyPawn : public APawn
{
    GENERATED_BODY()


        UPROPERTY(VisibleAnywhere, Category = Scene)
        USceneComponent *Root;

        UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        UStaticMeshComponent * Mesh;

public:
    // Sets default values for this pawn's properties
    AMyPawn();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    void LoadPak();

    void TryLoadPackageFromPak();

private:
    void LogAndPrintToScreen(const FString &Message, const FColor &MessageColor = FColor::Purple);

    struct FStreamableManager & GetStreamableManager();

    UObject *TryLoadAsset(const FString &AssetShortName);
};
