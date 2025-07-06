// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "LCExperienceManagerComponent.generated.h"

enum class ELCExperienceLoadState
{
	Unloaded,
	Loading,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLCExperienceLoaded, const class ULCExperienceDefinition*);

/**
ULCExperienceManagerComponent
- 말 그대로, 해당 컴포넌트는 GameState를 Owner로 가지면서 Experience의 상태 정보를 가지고 있는 컴포넌트이다.
- 뿐만 아니라, Manager라는 단어가 포함되어 있듯 Experience 로딩 상태 업데이트 및 이벤트 관리를 맡는다.

 
 */
UCLASS()
class LYRACLONE_API ULCExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	bool IsExperienceLoaded() const { return (LoadState == ELCExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	// OnExperienceLoaded에 바인딩하거나 Experience로딩이 완료되었다면 즉시 호출 
	void CallorRegister_OnExperienceLoaded(FOnLCExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnExperienceFullLoadCompleted();

public:
	UPROPERTY()
	TObjectPtr<const class ULCExperienceDefinition> CurrentExperience;

	// Experience상태 모니터링
	ELCExperienceLoadState LoadState = ELCExperienceLoadState::Unloaded;

	// Experience 로딩이 완료된 이후 Broadcasting Delegate
	FOnLCExperienceLoaded OnExperienceLoaded; 
};
