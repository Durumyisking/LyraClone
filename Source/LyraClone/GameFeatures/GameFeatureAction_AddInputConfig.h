#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Input/LCMappableConfigPair.h"
#include "GameFeatureAction_AddInputConfig.generated.h"


/** forward declarations */
struct FComponentRequestHandle;
class APawn;

UCLASS()
class UGameFeatureAction_AddInputConfig : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	/**
	 * UGameFetaureAction interface
	 */
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	/**
	 * UGameFeatureAction_WorldActionBase interface
	 */
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	/** GameFeatureState  */
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	/** InputConfig EnhancedInputLocalPlayerSubsystem */
	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);

	/** ActiveData를 초기화 */
	void Reset(FPerContextData& ActiveData);

	/** world -> (extension request, pawn) */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	UPROPERTY(EditAnywhere)
	TArray<FLCMappableConfigPair> InputConfigs;
};