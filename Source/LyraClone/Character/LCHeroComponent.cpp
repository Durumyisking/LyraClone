// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCHeroComponent.h"

ULCHeroComponent::ULCHeroComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}
