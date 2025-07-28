// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/LCInputConfig.h"
#include "LCGameplayTags.h"
#include "LCLog.h"

ULCInputConfig::ULCInputConfig(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

const UInputAction* ULCInputConfig::FindNativeInputActionForTag(const FGameplayTag InputTag, bool bLogNotFound) const
{
	// NativeInputActions를 순회하며 Input으로 들어온 InputTag가 있는지 체크한다.
	// - 있으면 그에 따른 InputAction을 반환하지만 없다면 그냥 nullptr 반환
	for (const FLCInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogLC, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}

const UInputAction* ULCInputConfig::FindAbilityInputActionForTag(const FGameplayTag InputTag, bool bLogNotFound) const
{
	for (const FLCInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}
	
	if (bLogNotFound)
	{
		UE_LOG(LogLC, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
