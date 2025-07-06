// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LCPlayerState.h"
#include "GameModes/LCExperienceDefinition.h"
#include "GameModes/LCExperienceManagerComponent.h"
#include "Character/LCPawnData.h"
#include "GameModes/LCGameModeBase.h"

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
	if (ALCGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ALCGameModeBase>())
	{
		const ULCPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check( NewPawnData);

		SetPawnData(NewPawnData);
	}
}


void ALCPlayerState::SetPawnData(const ULCPawnData* InPawnData)
{
	check(InPawnData);

	// PawnData가 두번 설정되는 것은 원하지 않음
	check(!PawnData);
	
	PawnData = InPawnData;
}
