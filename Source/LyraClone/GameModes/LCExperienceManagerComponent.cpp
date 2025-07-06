// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCExperienceManagerComponent.h"
#include "GameFeaturesSubsystemSettings.h"
#include "System/LCAssetManager.h"
#include "GameModes/LCExperienceDefinition.h"

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

void ULCExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	ULCAssetManager& AssetManager = ULCAssetManager::Get();
	
	TSubclassOf<ULCExperienceDefinition> AssetClass;
	{
		// ExperienceId가 스캔이 되어있으면 ExperienceDefinition에셋 경로 가져와서
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		// 동기로드
		AssetClass = Cast<UClass>(AssetPath.TryLoad());
	}

	// CDO가져오기
	const ULCExperienceDefinition* Experience = GetDefault<ULCExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr); // 첫 로딩인데 CurrentExperience가 있을 수 없음
	{
		// 그리고 CDO로 CurrentExperience를 설정한다.
		CurrentExperience = Experience;
	}
	StartExperienceLoad();
}

void ULCExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == ELCExperienceLoadState::Unloaded); // 로드 되어있으면 안된다
	
	LoadState = ELCExperienceLoadState::Loading;

	ULCAssetManager& AssetManager = ULCAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	// AssetManager.GetPrimaryAssetId()의 결과로
	// ALCGameModeBase::HandleMatchAssignmentIfNotExpectingOne 여기서 해준
	// FPrimaryAssetId(FPrimaryAssetType("LCExperienceDefinition"), FName("B_LCDefaultExperience"));
	// 해당 Id가 불러와 진다고 한다.
	// 근데 None이 불러와지는데..?
	BundleAssetList.Add(AssetManager.GetPrimaryAssetId());  

	// load assets associated with the experience
	// 아래는 우리가 후일 GameFeature를 사용하여, Experience에 바인딩된 GameFeature Plugin을 로딩할 Bundle 이름을 추가한다.
	// - Bundle
	/* 로딩할 에셋의 카테고리 이름
	 * 예를들어 BundleAssetList에 수많은 에셋들이 있는데 여기서 BundleName으로 분류된 특정 에셋들을 가져오고 싶으면
	 * AssetManager.ChangeBundleStateForPrimaryAssets함수의 인자 BundlesToLoad로 Bundle이름들을 전달하면
	 * 전달한 Bundle이름을 가진 Asset들이 BundleAssetList에 추가되게된다.
	 * (Ex: 직업으로 분류된 에셋리스트에서 "전사" 라는 번들을 넣으면 전사관련 에셋들만 불러올 수 있다. 
	*/
	
	TArray<FName> BundlesToLoad;
	{
		// 여기서 주목할 부분은 OwnerNetMode가 NM_Standalone이면 Client/Server 둘 다 로딩에 추가된다는 부분
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer); // 에디터거나 데디가 아니면
		bool bLoadServer  = GIsEditor || (OwnerNetMode != NM_Client); // 에디터거나 클라가 아니면
		if (bLoadClient)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient /*TEXT("Client")*/);
		}
		if (bLoadServer)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer /*TEXT("Server")*/);
		}
	}
	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	// 아래도, 후일 Bundle을 우리가 GameFeature에 연동하면서 더 깊게 알아보기로 하고 일단 디폴트Experience를 로딩해주는 함수라고 생각하자.
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets( // 번들을 비동기로 로딩해주는 함수래요
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// 로딩이 완료되었으면 ExecuteDelegate를 통해 OnAssetsLoadedDelegate를 호출
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		// 로딩 완료 안됐으면 
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate); // 로딩 완료됐을때 OnAssetsLoadedDelegate호출명령
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
		{
			// 만약 문제가 생겨서 취소 되었으면 OnAssetsLoadedDelegate 안전하게 호출
			OnAssetsLoadedDelegate.ExecuteIfBound();
		}));
	}
}

void ULCExperienceManagerComponent::OnExperienceLoadComplete()
{
	// check Framenubmer
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
	OnExperienceFullLoadCompleted();
}

void ULCExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	LoadState = ELCExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}
	