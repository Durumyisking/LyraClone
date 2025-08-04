// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LCUserFacingExperience.generated.h"

class UCommonSession_HostSessionRequest;
/**
 * 
 */
UCLASS()
class LYRACLONE_API ULCUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/*
	* Map로딩 및 Exp전환을 위해 MapId와 ExpId를 사용해서 HostSessionRequest생성한다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID; // Key Value 맵 말고 진짜 그 게임 맵ID

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "LCExperienceDefinition"))
	FPrimaryAssetId ExperienceID;

};
