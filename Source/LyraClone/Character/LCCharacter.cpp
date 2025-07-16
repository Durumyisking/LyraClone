// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCCharacter.h"
#include "LCPawnExtensionComponent.h"
#include "Camera/LCCameraComponent.h"

// Sets default values
ALCCharacter::ALCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	// create pawn ext Comp
	PawnExtensionComponent = CreateDefaultSubobject<ULCPawnExtensionComponent>("PawnExtensionComponent");;

	// create Camera Comp
	CameraComponent = CreateDefaultSubobject<ULCCameraComponent>("CameraComponent");;
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 75.f));
}

// Called when the game starts or when spawned
void ALCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 반드시 PlayerController가 존재하는 상태이기 때문에 Spawned->DataAvailable상태를 넘어갈 수 있다.
	PawnExtensionComponent->SetupPlayerInputComponent();
}

