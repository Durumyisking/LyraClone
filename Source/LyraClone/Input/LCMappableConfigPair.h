// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LCMappableConfigPair.generated.h"

class UPlayerMappableInputConfig;

USTRUCT()
struct FLCMappableConfigPair
{
	GENERATED_BODY()
public:
	FLCMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};
