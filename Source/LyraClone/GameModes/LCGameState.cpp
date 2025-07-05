// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCGameState.h"
#include "GameModes/LCExperienceManagerComponent.h"

ALCGameState::ALCGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<ULCExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
	
}
