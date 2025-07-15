// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCGameModeBase.h"

#include "LCExperienceDefinition.h"
#include "LCExperienceManagerComponent.h"
#include "LCGameState.h"
#include "LCLog.h"
#include "Player/LCPlayerController.h"
#include "Player/LCPlayerState.h"
#include "Character/LCCharacter.h"
#include "Character/LCPawnData.h"
#include "Character/LCPawnExtensionComponent.h"

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

UClass* ALCGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController를 활용해 PawnData로부터 PawnClass를 유도한다.
	if (const ULCPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ALCGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
		if (IsExperienceLoaded())
	{
		// 이거 실행 안하면 SpawnDefaultPawnAtTransform_Implementation같은 다음 프로세스의  함수들 호출이 되지 않는다.
		Super::HandleStartingNewPlayer_Implementation(NewPlayer); 
	}
}

APawn* ALCGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	UE_LOG(LogLC, Log, TEXT("SpawnDefaultPawnAtTransform_Implementation is Called! (Pawn Spawned On World!!)"));

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass * PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (ULCPawnExtensionComponent* PawnExtensionComponent = ULCPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const ULCPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtensionComponent->SetPawnData(PawnData);
				}
			}
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}
	
	return nullptr;
}

void ALCGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// 우리는 Experience로딩을 선택하는 과정에서만 쓸건데
	// 라이라에서는 지금 Experience가(데디서버든, 에디터모드든) 예상된 것이 들어왔는지 아닌지 확인하는 함수로 쓰인다고 한다.

	// 해당 함수에서는 우리가 로딩할 Experience에 대해 PrimaryAssetId를 생성하여, OnMatchAssignmentGiven으로 넘긴다.

	FPrimaryAssetId ExperienceId;

	// precedence order (highest wins)
	// - matchmaking assignment (if present)
	// - default experience

	UWorld* World = GetWorld();

	// fall back to the default experience
	// 일단 기본 옵션으로 default하게 B_LcDefaultExperience로 설정 하자
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("LCExperienceDefinition"), FName("B_LCDefaultExperience"));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void ALCGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// ExperienceManagerComponent로 Experience를 로딩하기 위해, ExperienceManagerComponent의 ServerSetCurrentExperience를 호출한다.
	check(ExperienceId.IsValid());

	ULCExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULCExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool ALCGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	ULCExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULCExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void ALCGameModeBase::OnExperienceLoaded(const class ULCExperienceDefinition* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController가 Pawn을 Possess하지 않았다면 RestartPawn을 통해 다시 spawn
		// 한 번 OnPossess를 보도록 하자
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

const ULCPawnData* ALCGameModeBase::GetPawnDataForController(const AController* InController) const
{
	// 게임 도중에 PawnData가 Override 되었을 경우, PawnData는 PlayerState에서 가져오게 됨
	if (InController)
	{
		if (const ALCPlayerState* LCPS = InController->GetPlayerState<ALCPlayerState>())
		{	
			if (const ULCPawnData* PawnData = LCPS->GetPawnData<ULCPawnData>())
			{
				return PawnData;	
			}
		}
	}

	// 아직 PlayerState에 PawnData가 없으면 ExperienceManagerComponent의 CurrentExperience를 가져와서 설정
	check(GameState);
	ULCExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<ULCExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const ULCExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperinenceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}
	return nullptr;
}
