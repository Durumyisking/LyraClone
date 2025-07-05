// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCCharacter.h"

// Sets default values
ALCCharacter::ALCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

