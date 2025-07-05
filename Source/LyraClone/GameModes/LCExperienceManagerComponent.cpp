// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCExperienceManagerComponent.h"

void ULCExperienceManagerComponent::CallorRegister_OnExperienceLoaded(FOnLCExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		/*
		 * Delegate 객체를 살펴보면, 내부적으로 필요한 변수들은 메모리 할당해놓는다
		 * TArray<int> a = {1,2,3}
		 * delegate_type delegate = [a]()
		 * {
		 *		return a.Num();
		 * }
		 * a는 delegate_type 내부에 동적할당 되어있다. 복사비용을 낮추기위해 Move사용하는것을 잊지말자
		 */
		
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}
	