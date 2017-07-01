// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"
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

    PlayerInputComponent->BindAction("TestA", IE_Pressed, this, &AMyPawn::HandleActionInputTestA);
    PlayerInputComponent->BindAction("TestB", IE_Pressed, this, &AMyPawn::HandleActionInputTestB);
}

void AMyPawn::HandleActionInputTestA()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("HandleActionInputTestA"));
}

void AMyPawn::HandleActionInputTestB()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("HandleActionInputTestB"));
}