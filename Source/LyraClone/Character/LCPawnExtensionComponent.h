// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "LCPawnExtensionComponent.generated.h"

/**
 * 초기화 전반을 조정하는 컴포넌트
 */
UCLASS()
class LYRACLONE_API ULCPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	ULCPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	static const FName NAME_ActorFeatureName;

	/*
	 * Member Method
	 */
	static ULCPawnExtensionComponent* FindPawnExtensionComponent(class AActor* Actor) { return Actor? Actor->FindComponentByClass<ULCPawnExtensionComponent>() : nullptr; }
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData);}
	void SetPawnData(const class ULCPawnData* InPawnData);
	void SetupPlayerInputComponent();
	/*
	 * pawn component interface
	 */
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/*
	 * IGameFrameworkInitStateInterface
	 */
	virtual FName GetFeatureName() const final {return NAME_ActorFeatureName;}
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;

	// PawnData 캐싱
	UPROPERTY(EditInstanceOnly, Category = "LC|Pawn")
	TObjectPtr<const class ULCPawnData> PawnData;
};
