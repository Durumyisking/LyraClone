#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"

struct FLCGameplayTags
{
	static const FLCGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(class UGameplayTagsManager& Manager);

	/*
	 *  아래의 Gameplaytags는 초기화 과정 단계를 의미한다.
	 *  - Gameinst의 초기화 과정에 UGameFrameworkComponentManager의 RegisterInitState로 등록되어 선형적으로 업데이트 된다
	 *  - 이 초기화 GameplayTag는 게임의 Actor 사이에 공유되며,
	 *  GameFrameworkInitStateInterface를 상속받은 클래스는 초기화 상태(Init State)를 상태머신(State Machine)과 같이 관리 가능한 인터페이스를 제공한다
	 */
	
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

private:
	static FLCGameplayTags GameplayTags; // 싱글톤으로 쓸거임
};