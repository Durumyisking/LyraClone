// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LYRACLONE_API ALCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALCPlayerController(const FObjectInitializer& ObjectInitializer);
};
