// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LCExperienceManagerComponent.h"

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesSubsystemSettings.h"
#include "LCExperienceActionSet.h"
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

void ULCExperienceManagerComponent::StartExperienceLoad() // Experience 에셋 로드
{
	check(CurrentExperience);
	check(LoadState == ELCExperienceLoadState::Unloaded); // 로드 되어있으면 안된다
	LoadState = ELCExperienceLoadState::Loading;

	ULCAssetManager& AssetManager = ULCAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	// CurrentExperience->GetPrimaryAssetId()의 결과로
	// ALCGameModeBase::HandleMatchAssignmentIfNotExpectingOne 여기서 해준
	// FPrimaryAssetId(FPrimaryAssetType("LCExperienceDefinition"), FName("B_LCDefaultExperience")); 해당 Id가 불러와 진다고 한다.
	// 왜냐하면 B_LCDefaultExperience가 ULCExperienceDefinition이고 CurrentExperience가 FPrimaryAssetType("LCExperienceDefinition"), FName("B_LCDefaultExperience")불러와서 만든거니까
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	// 만약 여기서 CurrentExperience의 에셋 클래스를 다른 BP로 바꾸면 GetPrimaryAssetId했을때 해당 BP받아와짐

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

																								// 에셋 로드 끝나면 여기로
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

void ULCExperienceManagerComponent::OnExperienceLoadComplete() // GameFeature로드 및 활성화
{
	// Look FrameNumber carefully
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	check(LoadState == ELCExperienceLoadState::Loading);
	check(CurrentExperience);

	// 이전 활성화된 GameFeature Plugin의 URL 클리어
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLS = [This = this] (const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	{
		// GameFeaturePluginList를 순회하며, PluginUYRL을 ExperienceManagerComponent의 GameFeaturePluginURLS에 추가해준다.
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL)) // 플러그인 이름 넣으면 플러그인 URL반환해주는 함수
			{
				This->GameFeaturePluginURLs.AddUnique(PluginURL);
			}
		}
	};

	// GameFeaturesToEnable에 있는 Plugin만 일단 활성화할 GameFeature Plugin 후보군으로 등록
	CollectGameFeaturePluginURLS(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	// GameFeaturePluginURL에 등록된 Plugin을 로딩 및 활성화
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if ( NumGameFeaturePluginsLoading )
	{
		LoadState = ELCExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			// 매 plugin이 로딩 및 활성화 이후 OnGameFeaturePluginLoadComplete 콜백 함수 등록															// 로드 끝나면 해당 함수 호출된다.
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this ,&ThisClass::OnGameFeaturePluginLoadComplete));
			
		}
	}
}

void ULCExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// 매 GameFeature plugin이 로딩될 때 이 함수가 콜백으로 불린다.
	NumGameFeaturePluginsLoading--;
	if (0 == NumGameFeaturePluginsLoading)
	{
		// GameFeaturePlugin 로딩과 활성화과 끝났다면 UGameFeatureAction을 활성화한다.
		OnExperienceFullLoadCompleted();
	}
	
	// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
}

void ULCExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ELCExperienceLoadState::Loaded);
	// 여기서 Action이 실행된다.
	// GameFeature Plugin의 로딩과 활성화 이후 GameFeature Action들을 활성화 시키자
	{
		LoadState = ELCExperienceLoadState::ExecutingActions;

		// GameFeatureAction 활성화를 위한 Context준비
		FGameFeatureActivatingContext Context;
		{
			// World의 Handle세팅

			UWorld* World = GetWorld();
 			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(World);
			if (ExistingWorldContext)
			{
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);	
			}
		}
		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>&	ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				// 명시적으로 GameFeatureAction에 대해 Registering -> Loading -> Activating순으로 호출한다.
				if (Action)
				{
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

		// 1. Experience의 actions
		ActivateListOfActions(CurrentExperience->Actions);

		// 2. Experience의 ActionSets
		for (const ULCExperienceActionSet* ActionSet : CurrentExperience->ActionSets)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}
	
	LoadState = ELCExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const ULCExperienceDefinition* ULCExperienceManagerComponent::GetCurrentExperinenceChecked() const
{
	check(LoadState==ELCExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
	