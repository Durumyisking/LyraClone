// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCHeroComponent.h"
#include "LCLog.h"
#include "LCGameplayTags.h"
#include "Character/LCPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/LCPlayerState.h"
#include "Character/LCPawnData.h"

const FName ULCHeroComponent::NAME_ActorFeatureName("Hero");

ULCHeroComponent::ULCHeroComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULCHeroComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 액터에 등록되었는지 확인
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogLC, Error, TEXT("this component has been added to a BP whose base class is not a pawn!!"));
	}

	RegisterInitStateFeature();
}

void ULCHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// ULCPawnExtensionComponent의 모든 상태를 추적한다.
	BindOnActorInitStateChanged(ULCPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FLCGameplayTags::Get().InitState_Spawned));

	// Forceupdate진행
	CheckDefaultInitialization();
}

void ULCHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature(); 
	
	Super::EndPlay(EndPlayReason);
}

void ULCHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	UE_LOG(LogLC, Log, TEXT("ULCHeroComponent::OnActorInitStateChanged [%s] [%s]"), *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
	if (Params.FeatureName == ULCPawnExtensionComponent::NAME_ActorFeatureName) 
	{
		// ULCPawnExtensionComponent의 State가 InitState_DataInitialized인것을 확인하고  HeroComponent도 InitState_DataInitialized로 변경한다..
		const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool ULCHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	// true 면 CurrentState에서 DesireState로 넘어가는 함수
	
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
	ALCPlayerState* LCPS = GetPlayerState<ALCPlayerState>();

	// InitState_Spawned초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn이 잘 세팅되어있으면 Spawned로 넘어간다.
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState== InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!LCPS)
		{
			return false;
		}
		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState== InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// ULCPawnExtensionComponent가 InitState_DataInitialized상태로 넘어갔으면 그때 this도 InitState_DataInitialized로 넘어감
		return LCPS &&  Manager->HasFeatureReachedInitState(Pawn, ULCPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState== InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void ULCHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();

	// DataAvailable -> DataInitialized성공 단계
	if (CurrentState == InitTags.InitState_DataAvailable && CurrentState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALCPlayerState* LCPS = GetPlayerState<ALCPlayerState>();
		if (!ensure(Pawn && LCPS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const ULCPawnData* PawnData = nullptr;
		if (ULCPawnExtensionComponent* PawnExtensionComponent = ULCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtensionComponent->GetPawnData<ULCPawnData>();
		}
	}
}

void ULCHeroComponent::CheckDefaultInitialization()
{
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}
