#include "CRPG_HUD.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "CRPG_CharacterBase.h" 
#include "CRPG_InventoryComponent.h"
#include "CRPG_InventoryWidget.h"
#include "CRPG_CameraRig.h"
#include "Kismet/KismetMathLibrary.h" 

ACRPG_HUD::ACRPG_HUD()
{
	bIsDialogueVisible = false;
}

void ACRPG_HUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHUDClass)
	{
		MainHUDInstance = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
		if (MainHUDInstance) MainHUDInstance->AddToViewport(0);
	}

	if (CombatWidgetClass)
	{
		CombatWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CombatWidgetClass);
		if (CombatWidgetInstance)
		{
			CombatWidgetInstance->AddToViewport(1);
			CombatWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (InventoryWidgetClass)
	{
		InventoryWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
		if (InventoryWidgetInstance)
		{
			InventoryWidgetInstance->AddToViewport(5);
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);

			APawn* PlayerPawn = GetOwningPlayerController() ? GetOwningPlayerController()->GetPawn() : nullptr;
			if (PlayerPawn)
			{
				UCRPG_InventoryComponent* InvComp = PlayerPawn->FindComponentByClass<UCRPG_InventoryComponent>();
				if (InvComp)
				{
					if (UCRPG_InventoryWidget* InvWidget = Cast<UCRPG_InventoryWidget>(InventoryWidgetInstance))
					{
						InvWidget->InitializeInventory(InvComp);
						UE_LOG(LogTemp, Log, TEXT("UI: Inventory Widget linked successfully."));
					}
				}
			}
		}
	}

	if (DialogueWidgetClass)
	{
		DialogueWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
		if (DialogueWidgetInstance)
		{
			DialogueWidgetInstance->AddToViewport(10);
			DialogueWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	SwitchToExplorationMode();
	UE_LOG(LogTemp, Log, TEXT("SILO-41 UI: HUD Initialized."));
}

void ACRPG_HUD::SwitchToExplorationMode()
{
	HideAllWidgets();

	if (MainHUDInstance)
	{
		MainHUDInstance->SetVisibility(ESlateVisibility::Visible);
	}

	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void ACRPG_HUD::SwitchToCombatMode()
{
	HideAllWidgets();
	if (CombatWidgetInstance) CombatWidgetInstance->SetVisibility(ESlateVisibility::Visible);
}

void ACRPG_HUD::ToggleInventory()
{
	if (!InventoryWidgetInstance) return;

	bool bIsVisible = InventoryWidgetInstance->IsVisible();
	APlayerController* PC = GetOwningPlayerController();

	ACRPG_CameraRig* CamRig = nullptr;
	if (PC)
	{
		AActor* ViewTarget = PC->GetViewTarget();
		CamRig = Cast<ACRPG_CameraRig>(ViewTarget);
	}

	if (bIsVisible)
	{
		// KAPAT
		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);

		if (CamRig) CamRig->DisableInventoryMode();

		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(InputMode);
		}
	}
	else
	{
		// AÇ
		UCRPG_InventoryWidget* InvWidget = Cast<UCRPG_InventoryWidget>(InventoryWidgetInstance);
		if (InvWidget)
		{
			InvWidget->ResetUI();
			InvWidget->RefreshInventory();
		}

		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);

		if (CamRig) CamRig->EnableInventoryMode();

		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;

			// Karakter Kameraya Dönsün
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				PC->StopMovement();
				FVector CharLoc = PlayerPawn->GetActorLocation();
				FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
				CamLoc.Z = CharLoc.Z;
				FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CharLoc, CamLoc);
				PlayerPawn->SetActorRotation(LookAtRot);
			}
		}
	}
}

void ACRPG_HUD::ShowDialogue(bool bShow, AActor* TargetNPC)
{
	if (!DialogueWidgetInstance) return;

	bIsDialogueVisible = bShow;
	DialogueWidgetInstance->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	APlayerController* PC = GetOwningPlayerController();
	ACRPG_CameraRig* CamRig = nullptr;
	if (PC)
	{
		AActor* ViewTarget = PC->GetViewTarget();
		CamRig = Cast<ACRPG_CameraRig>(ViewTarget);
	}

	if (bShow)
	{
		if (CamRig && TargetNPC) CamRig->EnableDialogueMode(TargetNPC);

		if (PC)
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(DialogueWidgetInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
	else
	{
		if (CamRig) CamRig->DisableDialogueMode();

		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

bool ACRPG_HUD::IsDialogueActive() const
{
	return bIsDialogueVisible;
}

// [YENÝ] Envanter kontrolü
bool ACRPG_HUD::IsInventoryActive() const
{
	if (InventoryWidgetInstance)
	{
		return InventoryWidgetInstance->IsVisible();
	}
	return false;
}

void ACRPG_HUD::HideAllWidgets()
{
	if (MainHUDInstance) MainHUDInstance->SetVisibility(ESlateVisibility::Collapsed);
	if (CombatWidgetInstance) CombatWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	if (InventoryWidgetInstance) InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	if (DialogueWidgetInstance) DialogueWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
}