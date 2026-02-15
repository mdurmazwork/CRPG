#include "CRPG_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "CRPG_CameraRig.h"
#include "CRPG_GameMode.h"     
#include "CRPG_GridManager.h"  
#include "CRPG_TurnManager.h"
#include "CRPG_InteractionComponent.h" 
#include "CRPG_InteractableActor.h"
#include "CRPG_CharacterBase.h"
#include "CRPG_HUD.h" 

ACRPG_PlayerController::ACRPG_PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bIsMiddleMousePressed = false;
	bIsTurnActive = true;

	bIsApproachingTarget = false;
	CachedTargetActor = nullptr;

	InteractionStoppingDistance = 180.0f;
}

void ACRPG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	TArray<AActor*> FoundRigs;
	UGameplayStatics::GetAllActorsOfClass(this, ACRPG_CameraRig::StaticClass(), FoundRigs);

	if (FoundRigs.Num() > 0)
	{
		ControlledCameraRig = Cast<ACRPG_CameraRig>(FoundRigs[0]);
		SetViewTarget(ControlledCameraRig);
	}
	SetIgnoreLookInput(true);
}

void ACRPG_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsApproachingTarget && CachedTargetActor)
	{
		CheckInteractionDistance();
	}
}

void ACRPG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveClickAction)
			EnhancedInputComponent->BindAction(MoveClickAction, ETriggerEvent::Triggered, this, &ACRPG_PlayerController::OnClickTriggered);

		if (HighlightAction)
		{
			EnhancedInputComponent->BindAction(HighlightAction, ETriggerEvent::Started, this, &ACRPG_PlayerController::OnHighlightPressed);
			EnhancedInputComponent->BindAction(HighlightAction, ETriggerEvent::Completed, this, &ACRPG_PlayerController::OnHighlightReleased);
		}

		if (InventoryAction)
		{
			EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ACRPG_PlayerController::OnInventoryKeyPressed);
		}

		if (EndTurnAction) EnhancedInputComponent->BindAction(EndTurnAction, ETriggerEvent::Started, this, &ACRPG_PlayerController::OnEndTurnTriggered);
		if (CameraPanAction) EnhancedInputComponent->BindAction(CameraPanAction, ETriggerEvent::Triggered, this, &ACRPG_PlayerController::OnCameraPan);
		if (CameraLockAction) EnhancedInputComponent->BindAction(CameraLockAction, ETriggerEvent::Started, this, &ACRPG_PlayerController::OnCameraLock);
		if (CameraRotateAction) EnhancedInputComponent->BindAction(CameraRotateAction, ETriggerEvent::Triggered, this, &ACRPG_PlayerController::OnCameraRotateKeyboard);
		if (CameraZoomAction) EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ACRPG_PlayerController::OnCameraZoom);
		if (RotateMouseHoldAction) {
			EnhancedInputComponent->BindAction(RotateMouseHoldAction, ETriggerEvent::Started, this, &ACRPG_PlayerController::OnRotateMouseHold);
			EnhancedInputComponent->BindAction(RotateMouseHoldAction, ETriggerEvent::Completed, this, &ACRPG_PlayerController::OnRotateMouseRelease);
		}
		if (RotateMouseAxisAction) EnhancedInputComponent->BindAction(RotateMouseAxisAction, ETriggerEvent::Triggered, this, &ACRPG_PlayerController::OnRotateMouseAxis);
	}
}

void ACRPG_PlayerController::SetIsTurnActive(bool bActive)
{
	bIsTurnActive = bActive;
}

void ACRPG_PlayerController::OnClickTriggered(const FInputActionValue& Value)
{
	if (bIsMiddleMousePressed) return;

	ACRPG_HUD* SiloHUD = Cast<ACRPG_HUD>(GetHUD());

	// [GÜNCELLEME] HAREKET ENGELLEYÝCÝ
	// Diyalog VEYA Envanter açýksa, dünyaya týklamayý engelle.
	if (SiloHUD)
	{
		if (SiloHUD->IsDialogueActive() || SiloHUD->IsInventoryActive())
		{
			return; // Hareketi iptal et
		}
	}

	ACRPG_GameMode* GM = Cast<ACRPG_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM) return;

	if (GM->GetCurrentGameState() == ECRPG_GameState::Combat && !bIsTurnActive) return;

	FHitResult Hit;
	bool bHitSuccessful = false;

	StopInteractionApproach();

	bHitSuccessful = GetHitResultUnderCursor(ECC_Visibility, true, Hit);

	if (!bHitSuccessful || (Hit.GetActor() && !Hit.GetActor()->IsA(ACRPG_InteractableActor::StaticClass()) && !Hit.GetActor()->IsA(ACRPG_CharacterBase::StaticClass())))
	{
		FHitResult PawnHit;
		if (GetHitResultUnderCursor(ECC_Pawn, true, PawnHit))
		{
			Hit = PawnHit;
			bHitSuccessful = true;
		}
	}

	if (!bHitSuccessful || !Hit.GetActor()) return;
	AActor* HitActor = Hit.GetActor();

	bool bIsInteractable = false;

	if (ACRPG_InteractableActor* ClickedObj = Cast<ACRPG_InteractableActor>(HitActor))
	{
		if (ClickedObj->bIsInteractable) bIsInteractable = true;
	}
	else if (ACRPG_CharacterBase* ClickedChar = Cast<ACRPG_CharacterBase>(HitActor))
	{
		if (!ClickedChar->IsPlayerCharacter()) bIsInteractable = true;
	}

	if (bIsInteractable && GM->GetCurrentGameState() == ECRPG_GameState::Exploration)
	{
		MoveToInteractable(HitActor);
		return;
	}

	if (GM->GetCurrentGameState() == ECRPG_GameState::Exploration)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Hit.Location);
	}
	else if (GM->GetCurrentGameState() == ECRPG_GameState::Combat)
	{
		AActor* GridActor = UGameplayStatics::GetActorOfClass(this, ACRPG_GridManager::StaticClass());
		ACRPG_GridManager* GridManager = Cast<ACRPG_GridManager>(GridActor);

		if (GridManager && GridManager->IsLocationInsideGrid(Hit.Location))
		{
			FVector TileCenter = GridManager->GetNearestTileLocation(Hit.Location);
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TileCenter);
		}
	}
}

void ACRPG_PlayerController::OnInventoryKeyPressed(const FInputActionValue& Value)
{
	ACRPG_HUD* SiloHUD = Cast<ACRPG_HUD>(GetHUD());
	if (SiloHUD)
	{
		SiloHUD->ToggleInventory();
	}
}

// ... Diðer fonksiyonlar ayný ...
void ACRPG_PlayerController::MoveToInteractable(AActor* Target)
{
	if (!Target || !GetPawn()) return;
	CachedTargetActor = Target;
	bIsApproachingTarget = true;
	FVector Origin;
	FVector BoxExtent;
	Target->GetActorBounds(true, Origin, BoxExtent);
	float TargetRadius = FMath::Max(BoxExtent.X, BoxExtent.Y);
	FVector MyLoc = GetPawn()->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();
	FVector DirToTarget = (TargetLoc - MyLoc).GetSafeNormal2D();
	float OffsetDistance = TargetRadius + 70.0f;
	FVector MovePos = TargetLoc - (DirToTarget * OffsetDistance);
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MovePos);
}

void ACRPG_PlayerController::CheckInteractionDistance()
{
	if (!CachedTargetActor || !GetPawn())
	{
		StopInteractionApproach();
		return;
	}
	float DistToCenter = FVector::Dist(GetPawn()->GetActorLocation(), CachedTargetActor->GetActorLocation());
	FVector Origin;
	FVector BoxExtent;
	CachedTargetActor->GetActorBounds(true, Origin, BoxExtent);
	float TargetRadius = FMath::Max(BoxExtent.X, BoxExtent.Y);
	float AcceptableDist = InteractionStoppingDistance + TargetRadius;
	if (DistToCenter <= AcceptableDist)
	{
		StopMovement();
		PerformInteraction();
		StopInteractionApproach();
	}
}

void ACRPG_PlayerController::PerformInteraction()
{
	if (!CachedTargetActor) return;
	if (ACRPG_InteractableActor* Interactable = Cast<ACRPG_InteractableActor>(CachedTargetActor))
	{
		Interactable->OnInteract(GetPawn());
	}
	else if (ACRPG_CharacterBase* TargetChar = Cast<ACRPG_CharacterBase>(CachedTargetActor))
	{
		TargetChar->InteractWithCharacter();
	}
}

void ACRPG_PlayerController::StopInteractionApproach()
{
	bIsApproachingTarget = false;
	CachedTargetActor = nullptr;
}

void ACRPG_PlayerController::OnHighlightPressed(const FInputActionValue& Value)
{
	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		UCRPG_InteractionComponent* InteractComp = MyPawn->FindComponentByClass<UCRPG_InteractionComponent>();
		if (InteractComp) InteractComp->ToggleHighlightMode(true);
	}
}

void ACRPG_PlayerController::OnHighlightReleased(const FInputActionValue& Value)
{
	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		UCRPG_InteractionComponent* InteractComp = MyPawn->FindComponentByClass<UCRPG_InteractionComponent>();
		if (InteractComp) InteractComp->ToggleHighlightMode(false);
	}
}

void ACRPG_PlayerController::OnEndTurnTriggered(const FInputActionValue& Value)
{
	ACRPG_GameMode* GM = Cast<ACRPG_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM || GM->GetCurrentGameState() != ECRPG_GameState::Combat) return;
	if (!bIsTurnActive) return;

	AActor* TM_Actor = UGameplayStatics::GetActorOfClass(this, ACRPG_TurnManager::StaticClass());
	if (ACRPG_TurnManager* TM = Cast<ACRPG_TurnManager>(TM_Actor))
	{
		SetIsTurnActive(false);
		TM->NextTurn();
	}
}

void ACRPG_PlayerController::OnCameraPan(const FInputActionValue& Value)
{
	FVector2D PanVector = Value.Get<FVector2D>();
	if (ControlledCameraRig) ControlledCameraRig->AddPanInput(PanVector);
}

void ACRPG_PlayerController::OnCameraLock(const FInputActionValue& Value)
{
	if (ControlledCameraRig) ControlledCameraRig->ToggleCameraLock();
}

void ACRPG_PlayerController::OnCameraRotateKeyboard(const FInputActionValue& Value)
{
	float RotateValue = Value.Get<float>();
	if (ControlledCameraRig)
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		ControlledCameraRig->AddYawInput(RotateValue * DeltaTime * KeyboardRotationSpeed);
	}
}

void ACRPG_PlayerController::OnCameraZoom(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();
	if (ControlledCameraRig) ControlledCameraRig->AddZoomInput(ZoomValue);
}

void ACRPG_PlayerController::OnRotateMouseHold(const FInputActionValue& Value)
{
	bIsMiddleMousePressed = true;
}

void ACRPG_PlayerController::OnRotateMouseRelease(const FInputActionValue& Value)
{
	bIsMiddleMousePressed = false;
}

void ACRPG_PlayerController::OnRotateMouseAxis(const FInputActionValue& Value)
{
	if (!bIsMiddleMousePressed) return;
	float MouseDelta = Value.Get<float>();
	if (ControlledCameraRig) ControlledCameraRig->AddYawInput(MouseDelta * MouseRotationSpeed);
}