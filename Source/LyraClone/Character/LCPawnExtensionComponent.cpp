// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCPawnExtensionComponent.h"

#include "LCGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "LCLog.h"

const FName ULCPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

ULCPawnExtensionComponent::ULCPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULCPawnExtensionComponent::SetPawnData(const ULCPawnData* InPawnData)
{
	// Pawn에 대해 Authority가 없을경우 SetSpawnData X
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	PawnData = InPawnData;
}

void ULCPawnExtensionComponent::SetupPlayerInputComponent()
{
	// InitStateForceUpdate
	CheckDefaultInitialization();
}

void ULCPawnExtensionComponent::OnRegister() 
{
	Super::OnRegister();
	// 생성 시점의 매우 초반에 호출된다.
	// OnRegister에서 Pawn에 부착하는 단계가 이루어지기때문에 아래 코드는 실패할수가 없다.

	// 올바른 액터에 등록되었는지 확인
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogLC, Error, TEXT("this component has been added to a BP whose base class is not a pawn!!"));
	}

	// GameFrameworkComponentManager에 InitState 사용을 위해 '등록'진행
	// - 등록은 상속받았던 IGameFrameworkInitStateInterface::RegisterInitStateFeature() 를 활용
	RegisterInitStateFeature();

	// 디버깅
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor()); 
}

void ULCPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 아래 함수의 FeatureName에 Name_None을 넣으면 Actor에 등록된 모든 Feature의 InitState 상태를 관찰하겠다는 의미이다.
	// 특정 컴포넌트만 관찰할거면 해당 컴포넌트의 FeatureName넣으면 될 것
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	// 내부 함수를 보니까 Register에 성공했을때 IGameFrameworkInitStateInterface::OnActorInitStateChanged를 호출하는 델리게이트를
	// ActorInitStateChangedHandle에 등록한다. (가능하면 즉시호출도 하는걸로 보임)

	// 다른 예시로
	// BindOnActorInitStateChanged(NAME_None, FLCGameplayTags::Get().InitState_DataInitialized(), false);
	// 이렇게 하면 내 모든 Feature(Component)들을 관찰을 할건데 InitState_DataInitialized단계로 넘어가는 컴포넌트만 관찰을 해달라고 요청하는 것

	// ULCPawnExtensionComponent는 모든 Feature에 대한 모든 State를 관리하는 컴포넌트니까 위의 코드처럼 작성한다.

	// InitState_Spawned로 상태 변화
	// TryToChangeInitState는 인자로 넣은 State로 상태변화를 시도한다 (실패 가능성도 있다!)
	// -TryToChangeInitState는 아래와 같이 진행된다.
	// 1. CanChangeInitState로 상태 변환 가능성 유무 판단.
	// 2. HandleChangeInitState로 내부 상태 변경 (Feature Component)
	// 3. 위에서 BindOnActorInitStateChanged로 Bind된 Delegate를 조건에 맞게 호출해 줌
	//		- LCPawnExtensionComponent의 경우, 모든 Actor의 Feature 상태 변화에 대해 OnActorInitStateChanged()가 호출됨
	ensure(TryToChangeInitState(FLCGameplayTags::Get().InitState_Spawned));

	// ForceUpdateInitState를 하는 함수이다 (CanChangeInitState와 HandleChangeInitState를 진행한다.).
	CheckDefaultInitialization();
}

void ULCPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// OnRegister에서 등록해준것처럼 여기서 등록을 해제한다.
	UnregisterInitStateFeature(); // 이거 안하면 운없으면 GC에 안걸려서 InitState관련이 메모리 해제 안될수도있다.
	
	Super::EndPlay(EndPlayReason);
}

void ULCPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	UE_LOG(LogLC, Log, TEXT("ULCPawnExtensionComponent::OnActorInitStateChanged [%s] [%s]"), *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
	if (Params.FeatureName != NAME_ActorFeatureName) // 하위 Feature들만 받겠다는 뜻
	{
		// ULCPawnExtensionComponent::CanChangeInitState에 다른 Feature들의 상태가 DataAvailable로 변하는지 관찰하여 Sync를 맞추는 구간이 있다.
		// 이를 가능하게 하기 위해 여기서 지속적으로 CheckDefaultInitialization을 호출하여 상태를 확인한다.
		const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool ULCPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	// true 면 CurrentState에서 DesireState로 넘어가는 함수
	
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();

	// InitState_Spawned초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn이 잘 세팅되어있으면 Spawned로 넘어간다.
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState== InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (!PawnData)
		{
			return false;
		}

		// LocallyControlled인 Pawn이 Controller가 없으면 에러
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (!bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}
		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState== InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Pawn에 바인드된 모든 Feature들이 DataAvailable상태일 때 DataInitialized로 넘어감.
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState== InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void ULCPawnExtensionComponent::CheckDefaultInitialization()
{
	// ULCPawnExtensionComponent은 FeatureComponent들의 초기화를 관장하는 Component이다.
	// - 따라서 Actor에 바인딩된 Feature Comp들에 대해 CheckDefaultInitialization을 호출하게 한다. (바인딩된 FComp들에 대해 강제로 초기화 시키는 느낌)
	// 위의 기능은 Interface내에 CheckDefaultInitializationForImplementers를 호출하면 수행된다.
	// Implementers는 FeatureComp를 의미한다.
	CheckDefaultInitializationForImplementers();

	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();

	// 사용자 정의 InitState를 직접 넘겨줘야 한다.
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// CanChangeInitState()와 HandleChangeInitState() 그리고 ChangeFeatureInitState 호출을 통한 OnActorInitStateChanged Delegate호출까지 진행해준다

	ContinueInitStateChain(StateChain);
}
