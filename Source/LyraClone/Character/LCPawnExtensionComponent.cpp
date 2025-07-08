// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "LCLog.h"

const FName ULCPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

ULCPawnExtensionComponent::ULCPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULCPawnExtensionComponent::OnRegister() 
{
	Super::OnRegister();
	// 생성 시점의 매우 초반에 호출된다.
	// OnRegister에서 Pawn에 부착하는 단계가 이루어지기때문에 아래 코드는 실패할수가 없다.

	// 올바른 액터에 등록되었는지 확인
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogLC, Error, TEXT("this component has been added to a BP whose base class is not a pawn!!"));
	}

	// GameFrameworkComponentManager에 InitState 사용을 위해 '등록'진행
	// - 등록은 상속받았던 IGameFrameworkInitStateInteface::RegisterInitStateFeature() 를 활용
	RegisterInitStateFeature();

	// 디버깅
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor()); 
}

void ULCPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULCPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
