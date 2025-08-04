// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonSessionSubsystem.h"
#include "Engine/AssetManager.h"

FString UCommonSession_HostSessionRequest::GetMapName() const
{
	// MapID으로부터 AssetData를 반환하고 AssetData내의 PackageName을 통해 Map 경로 반환
	FAssetData MapAssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(MapID, MapAssetData))
	{
		return MapAssetData.PackageName.ToString();
	}
	return FString();
}

FString UCommonSession_HostSessionRequest::ConstructTravelURL() const
{
	/*
	 * Map : MyMap
	 * Exp : FPSMode
	 * result : MyMap?Experience=FPSMode
	 */
	
	FString CombinedExtraArgs;
	for (const auto& ExtraArg : ExtraArgs)
	{
		if (ExtraArg.Key.IsEmpty())
		{
			continue;
		}

		/*
		 * ?를 Separate(구분자)로 복수개의 ExtraArgs를 추가
		 *  Key값 유무에 따라 =(Assignment)를 통해 알맞는 Cmdarg생성
		 *  ?뒤의 값은 Arg임
		 */
		if (ExtraArg.Value.IsEmpty())
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s"), *ExtraArg.Key);
		}
		else
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s=%s"), *ExtraArg.Key, *ExtraArg.Value);
		}
	}
	return FString::Printf(TEXT("%s%s"), *GetMapName(), *CombinedExtraArgs);
}

void UCommonSessionSubsystem::HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request)
{
	ULocalPlayer* LocalPlayer = (nullptr != HostingPlayer) ? HostingPlayer->GetLocalPlayer() : nullptr;
	if (!LocalPlayer)
	{
		return;
	}

	// Lyar원본에서는 ClientTravel을 통해 자기자신이 다른 서버로 접속할 수 있도록 해준다.
	// HostingPlayer->ClientTravel()

	// 우리는 로컬환경이니까 월드에서 이동
	// HostSessionRequest에서 MapID와 ExtraArgs를 통해 URL생성하여, MapLoad를 시작
	GetWorld()->ServerTravel(Request->ConstructTravelURL());
}
