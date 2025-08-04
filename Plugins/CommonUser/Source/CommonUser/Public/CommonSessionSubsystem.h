// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubsystem.generated.h"

/**
 * UCommonSession_HostSessionRequest 만들어 UCommonSessionSubsystem 에 전달하면 UCommonSessionSubsystem 에서 MapID와 CmdArgs로 최종적인 CmdArgs를 만들어준다
 */

UCLASS(BlueprintType)
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()
public:
	UCommonSession_HostSessionRequest() {}
	
	// MapID -> Text변환
	FString GetMapName() const;

	// ServerTravel에 전달할 최종 URL생성
	FString ConstructTravelURL() const;

	// 준비할 Map 경로
	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	// 전달할 CmdArgs (Experience 이름 전달)
	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
	
};

UCLASS()
class COMMONUSER_API UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCommonSessionSubsystem() {}

	UFUNCTION(BlueprintCallable, Category = Session)
	void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);

	// 맵의 경로
	FString PendingTravelURL;
	
};
