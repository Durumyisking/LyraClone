// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "LCInputConfig.h"
#include "LCInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ULCInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	ULCInputComponent(const FObjectInitializer& ObjectInitializer);


	/*
	 * Member Methods
	 */
	template <class UserClass, typename FuncType>
	void BindNativeAction(const ULCInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound = true);

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const ULCInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
};

// InputConfig에서 InputAction을 Tag로 찾아온 뒤 해당 InputAction에 함수와 트리거 이벤트들을 바인딩한다.
template <class UserClass, typename FuncType>
void ULCInputComponent::BindNativeAction(const ULCInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void ULCInputComponent::BindAbilityActions(const ULCInputConfig* InputConfig, const FGameplayTag& InputTag,
	UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	// AbilityAction에 대해서는 그냥 모든 InputAction에 다 바인딩 시킨다.
	for (const FLCInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
