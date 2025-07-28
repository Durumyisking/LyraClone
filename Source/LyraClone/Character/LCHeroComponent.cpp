// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LCHeroComponent.h"
#include "LCLog.h"
#include "LCGameplayTags.h"
#include "Camera/LCCameraComponent.h"
#include "Character/LCPawnExtensionComponent.h"
#include "PlayerMappableInputConfig.h"
#include "Input/LCMappableConfigPair.h"
#include "Input/LCInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/LCPlayerState.h"
#include "Character/LCPawnData.h"
#include "Player/LCPlayerController.h"

const FName ULCHeroComponent::NAME_ActorFeatureName("Hero");

ULCHeroComponent::ULCHeroComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULCHeroComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 액터에 등록되었는지 확인
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogLC, Error, TEXT("this component has been added to a BP whose base class is not a pawn!!"));
	}

	RegisterInitStateFeature();
}

void ULCHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// ULCPawnExtensionComponent의 모든 상태를 추적한다.
	BindOnActorInitStateChanged(ULCPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FLCGameplayTags::Get().InitState_Spawned));

	// Forceupdate진행
	CheckDefaultInitialization();
}

void ULCHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature(); 
	
	Super::EndPlay(EndPlayReason);
}

void ULCHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	UE_LOG(LogLC, Log, TEXT("ULCHeroComponent::OnActorInitStateChanged [%s] [%s]"), *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
	if (Params.FeatureName == ULCPawnExtensionComponent::NAME_ActorFeatureName) 
	{
		// ULCPawnExtensionComponent의 State가 InitState_DataInitialized인것을 확인하고  HeroComponent도 InitState_DataInitialized로 변경한다..
		const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool ULCHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	// true 면 CurrentState에서 DesireState로 넘어가는 함수
	
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
	ALCPlayerState* LCPS = GetPlayerState<ALCPlayerState>();

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
		if (!LCPS)
		{
			return false;
		}
		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState== InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// ULCPawnExtensionComponent가 InitState_DataInitialized상태로 넘어갔으면 그때 this도 InitState_DataInitialized로 넘어감
		return LCPS &&  Manager->HasFeatureReachedInitState(Pawn, ULCPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState== InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void ULCHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();

	// DataAvailable -> DataInitialized성공 단계
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALCPlayerState* LCPS = GetPlayerState<ALCPlayerState>();
		if (!ensure(Pawn && LCPS))
		{
			return;
		}

		// Input, Camera Handling
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const ULCPawnData* PawnData = nullptr;
		if (ULCPawnExtensionComponent* PawnExtensionComponent = ULCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtensionComponent->GetPawnData<ULCPawnData>();
		}
		if (bIsLocallyControlled && PawnData)
		{
			if ( ULCCameraComponent* CameraComponent = ULCCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		if (ALCPlayerController* LCPC = GetController<ALCPlayerController>())
		{
			if (nullptr != Pawn->InputComponent)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
		
	}
}

void ULCHeroComponent::CheckDefaultInitialization()
{
	const FLCGameplayTags& InitTags = FLCGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

TSubclassOf< ULCCameraMode> ULCHeroComponent::DetermineCameraMode() const
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (ULCPawnExtensionComponent* PawnExtensionComponent = ULCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULCPawnData* PawnData = PawnExtensionComponent->GetPawnData<ULCPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}
	if (ULCPawnExtensionComponent* PawnExtensionComponent = ULCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULCPawnData* PawnData = PawnExtensionComponent->GetPawnData<ULCPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}
	return nullptr;
}


void ULCHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// Localplayer을 가져오기 위함
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem을 가져오기 위함
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* SubSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(SubSystem);

	// EnhancedInputLocalPlayerSubsystem의 MappingContext를 비워준다.
	SubSystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData - >InputConfig 존재 유무 판단.
	if (ULCPawnExtensionComponent* PawnExtensionComponent = ULCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULCPawnData* PawnData = PawnExtensionComponent->GetPawnData<ULCPawnData>())
		{
			if (const ULCInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FLCGameplayTags GameplayTags = FLCGameplayTags::Get();

				// HeroComponent가지고 있는 InputMapping Context를 순회하며 EnhancedInputLocalPlayerSubsystem에 추가
				for (const FLCMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically)
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						// 내부적으로 Input Mapping Context를 추가한다.
						SubSystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
					}
				}
				ULCInputComponent* LCIC = CastChecked<ULCInputComponent>(PlayerInputComponent);
				{
					LCIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this ,&ThisClass::Input_Move, false);
					LCIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this ,&ThisClass::Input_LookMouse, false);
				}

			}
		}
	}
}

void ULCHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			// Left/Right -> X 값에 들어있음:
			// MovementDirection은 현재 카메라의 RightVector를 의미함 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovementInput 함수를 한번 보자:
			// - 내부적으로 MovementDirection * Value.X를 MovementComponent에 적용(더하기)해준다
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f) // 앞서 우리는 Forward 적용을 위해 swizzle input modifier를 사용했다~
		{
			// 앞서 Left/Right와 마찬가지로 Forward/Backward를 적용한다
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void ULCHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// X에는 Yaw 값이 있음:
		// - Camera에 대해 Yaw 적용
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y에는 Pitch 값!
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}
