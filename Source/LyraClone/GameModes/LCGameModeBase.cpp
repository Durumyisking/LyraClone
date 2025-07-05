// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCGameModeBase.h"
#include "LCExperienceManagerComponent.h"
#include "LCGameState.h"
#include "Player/LCPlayerController.h"
#include "Player/LCPlayerState.h"
#include "Character/LCCharacter.h"

ALCGameModeBase::ALCGameModeBase()
{
	GameStateClass = ALCGameState::StaticClass();
	PlayerControllerClass = ALCPlayerController::StaticClass();
	PlayerStateClass = ALCPlayerState::StaticClass();
	DefaultPawnClass = ALCCharacter::StaticClass();
	
}

void ALCGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);


	// 한 프레임 뒤로 미루는 이유는
	// Experience의 로딩은 ExperienceManagerComponent에서 맡는데 Init 게임 시점에서는 해당 컴포넌트를 가지는 액터가 스폰되어있지 않다.
	// 그래서 스폰이후로 시점을 잡기위해 타이머로 한 틱 미룬다.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
	
}

void ALCGameModeBase::InitGameState()
{
	// GameState생성 이후에 호출되는 함수
	Super::InitGameState();

	// Experience 비동기 로딩을 위한 Delegate 준비한다.
	ULCExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULCExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded 등록
	ExperienceManagerComponent->CallorRegister_OnExperienceLoaded((FOnLCExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded)));
}

void ALCGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// 우리는 Experience로딩을 선택하는 과정에서만 쓸건데
	// 라이라에서는 지금 Experience가(데디서버든, 에디터모드든) 예상된 것이 들어왔는지 아닌지 확인하는 함수로 쓰인다고 한다.
	
}

void ALCGameModeBase::OnExperienceLoaded(const class ULCExperienceDefinition* CurrentExperience)
{
}
