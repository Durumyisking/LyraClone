// Fill out your copyright notice in the Description page of Project Settings.


#include "System/LCAssetManager.h"
#include "LCLog.h"
#include "LCGameplayTags.h"

ULCAssetManager::ULCAssetManager()
{
}

void ULCAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FLCGameplayTags::InitializeNativeTags(); // 에디터도 실행 전에 호출된다.
}

ULCAssetManager& ULCAssetManager::Get()
{
	ULCAssetManager* Singleton = Cast<ULCAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogLC, Fatal, TEXT("Cannot use AssetManager if no AssetManagerClassName is defined!"));
		return *NewObject<ULCAssetManager>(); // never calls this
	}
}

bool ULCAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads")); // cmd의 CommandLine에 "LogAssetLoads"라는 커맨드가 호출되었는지 확인한다. 있으면 true 반환
	return false;
}

UObject* ULCAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// 이 함수는 동기로딩을 진행할때 느려지거나 한다는 문제가 생겼을때 확인하기 위해서 만든 함수이다.

	if (AssetPath.IsValid())
	{
		// FScopeLogTime을 확인
		TUniquePtr<FScopeLogTime> LogTimePtr;

		// CommandLine에 LogAssetLoads를 넣으면 동기 로딩을 진행할때마다 아래에 로그를 찍게 되는것이다. 
		// 로그찍기싫으면 빼면된다.
		if (ShouldLogAssetLoads()) 
		{
			// 이 함수가 얼마나 시간을 소모했는지 로깅한다.
			// 이를 통해 AssetPath를 로드하는데 얼마나 시간이 걸렸는지 알 수 있음
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// AssetManager가 있으면 AssetManager의 StreamableManager을 통해 정적 로딩
		// StreamableManager : 동기/비동기 로딩을 관리하는 매니저
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// AssetManager가 없으면
		// TryLoad는 매우 느리기때문에 사용을 지양한다.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

void ULCAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}

}
