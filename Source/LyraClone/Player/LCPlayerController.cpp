// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LCPlayerController.h"
#include "Camera/LCPlayerCameraManager.h"

ALCPlayerController::ALCPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ALCPlayerCameraManager::StaticClass();
}
