// Fill out your copyright notice in the Description page of Project Settings.


#include "System/LCGameInstance.h"
#include "Components/GameFrameworkComponentManager.h" // ModularGameplayActor Plugin있어야 사용
#include "LCGameplayTags.h"

void ULCGameInstance::Init()
{
	Super::Init();

	// 앞서 정의한 InitState의 GameplayTags 등록:
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		const FLCGameplayTags& GameplayTags = FLCGameplayTags::Get();

		// RegisterInitState이거 하기위해 cpp에서 GameplayTags만들어서 사용한것
		// RegisterInitState함수는 언리얼 문서에 GameInstance::Init시점에서 해야한다고 나와있다.
		// 이 GameplayTags를 추가한 이유는 Component들을 초기화 할때 아래의 4단계의 순서에 맞춰서 컴포넌트들을 초기화 하기 위해서 사용한다고한다..
		// 일단은 패쓰/// 쓰면서 알아보자
		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
		
	}
}

void ULCGameInstance::Shutdown()
{
	Super::Shutdown();
}
