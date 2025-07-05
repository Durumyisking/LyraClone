// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LCPlayerState.h"
#include "GameModes/LCExperienceDefinition.h"
#include "GameModes/LCExperienceManagerComponent.h"

void ALCPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//  AGameStateBase이거 무조건 있다.
	const AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	ULCExperienceManagerComponent* ExperienceManagerComponent =GameState->FindComponentByClass<ULCExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallorRegister_OnExperienceLoaded(FOnLCExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ALCPlayerState::OnExperienceLoaded(const class ULCExperienceDefinition* CurrentExperience)
{
//	CurrentExperience->
	
}
