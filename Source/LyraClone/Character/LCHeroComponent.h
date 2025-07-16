// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "LCHeroComponent.generated.h"

/**
 *  카메라, 입력 등 플레이어가 제어하는 시스템의 초기화를 처리하는 컴포넌트
 *  일반적으로는 플레이어가 사용한는 캐릭터에 컴포넌트를 달아주는 방식으로 했을텐데
 *  그럼 플레이어 캐릭터와 Input/Camera등에 종속성이 생기게 된다.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class LYRACLONE_API ULCHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	ULCHeroComponent(const FObjectInitializer& ObjectInitializer);

	static const FName NAME_ActorFeatureName;

	// UPawnComponent Interface
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	// IGameFrameworkInitStateInterface  Interface
	virtual FName GetFeatureName() const final {return NAME_ActorFeatureName;}
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)final;
	virtual void CheckDefaultInitialization() final;

	// Member Methods
	TSubclassOf<class ULCCameraMode> DetermineCameraMode() const;
};
