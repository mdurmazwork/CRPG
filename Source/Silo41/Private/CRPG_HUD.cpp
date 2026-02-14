#include "CRPG_HUD.h"
#include "Kismet/GameplayStatics.h"

ACRPG_HUD::ACRPG_HUD()
{
	// Varsayýlan deðerler
}

void ACRPG_HUD::BeginPlay()
{
	Super::BeginPlay();

	// 1. Widget'larý Yarat (Viewport'a ekle ama gizle)
	if (MainHUDClass)
	{
		MainHUDInstance = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
		if (MainHUDInstance) MainHUDInstance->AddToViewport(0); // Z-Order 0 (En alt)
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
			InventoryWidgetInstance->AddToViewport(5); // Envanter üstte dursun
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (DialogueWidgetClass)
	{
		DialogueWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), DialogueWidgetClass);
		if (DialogueWidgetInstance)
		{
			DialogueWidgetInstance->AddToViewport(10); // Diyalog en üstte
			DialogueWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 2. Baþlangýç Modunu Ayarla
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

	// Mouse kontrolü PlayerController'da yönetiliyor ancak UI için input modu burada ayarlanabilir.
}

void ACRPG_HUD::SwitchToCombatMode()
{
	HideAllWidgets();

	// Savaþta MainHUD kalabilir veya gidebilir, tasarýma baðlý. Þimdilik sadece Combat'ý açalým.
	if (CombatWidgetInstance)
	{
		CombatWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACRPG_HUD::ToggleInventory()
{
	if (!InventoryWidgetInstance) return;

	bool bIsVisible = InventoryWidgetInstance->IsVisible();

	if (bIsVisible)
	{
		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		// Oyuna dön
	}
	else
	{
		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		// Mouse'u serbest býrak, oyunu duraklat vs.
	}
}

void ACRPG_HUD::ShowDialogue(bool bShow)
{
	if (!DialogueWidgetInstance) return;

	DialogueWidgetInstance->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ACRPG_HUD::HideAllWidgets()
{
	if (MainHUDInstance) MainHUDInstance->SetVisibility(ESlateVisibility::Collapsed);
	if (CombatWidgetInstance) CombatWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	// Inventory ve Dialogue genelde modal olduðu için bu toplu temizliðe dahil etmeyebiliriz, 
	// ama "Mod Deðiþimi" sýrasýnda temizlemek güvenlidir.
	if (InventoryWidgetInstance) InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	if (DialogueWidgetInstance) DialogueWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
}