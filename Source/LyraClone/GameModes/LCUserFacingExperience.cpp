// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCUserFacingExperience.h"
#include "CommonSessionSubsystem.h"

UCommonSession_HostSessionRequest* ULCUserFacingExperience::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();
	
	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}
