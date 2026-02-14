#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "CRPG_HUD.generated.h"

/**
 * ACRPG_HUD
 * * UI YÖNETÝCÝSÝ
 * Görevi: Oyun açýldýðýnda gerekli tüm Widget'larý (HUD, Inventory, Combat) yaratýr.
 * GameMode veya PlayerController'dan gelen emre göre sayfalar arasý geçiþ yapar.
 */
UCLASS()
class SILO41_API ACRPG_HUD : public AHUD
{
	GENERATED_BODY()

public:
	ACRPG_HUD();

	// --- CONFIG (Editörden Widget Seçimi) ---

	// Ana Oyun Ekraný (Can Barý, Mini-map vs.)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> MainHUDClass;

	// Envanter Ekraný
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	// Savaþ Arayüzü (AP Barý, Turn Order)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> CombatWidgetClass;

	// Diyalog Kutusu
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> DialogueWidgetClass;

	// --- CONTROL API (C++ Çaðrýlarý) ---

	// Oyunu keþif moduna alýr (Inventory ve Combat kapanýr, MainHUD açýlýr)
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void SwitchToExplorationMode();

	// Savaþ arayüzünü açar
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void SwitchToCombatMode();

	// Envanteri açar/kapatýr (Toggle)
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void ToggleInventory();

	// Diyalog kutusunu gösterir/gizler
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void ShowDialogue(bool bShow);

	// YENÝ: Widget örneklerine eriþim (Diðer sýnýflar veriyi buraya gönderecek)
	UUserWidget* GetCombatWidget() const { return CombatWidgetInstance; }
	UUserWidget* GetInventoryWidget() const { return InventoryWidgetInstance; }
	UUserWidget* GetDialogueWidget() const { return DialogueWidgetInstance; }

protected:
	virtual void BeginPlay() override;

private:
	// --- INSTANCES (Yaratýlan Widgetlar) ---
	UPROPERTY()
	UUserWidget* MainHUDInstance;

	UPROPERTY()
	UUserWidget* InventoryWidgetInstance;

	UPROPERTY()
	UUserWidget* CombatWidgetInstance;

	UPROPERTY()
	UUserWidget* DialogueWidgetInstance;

	// Helper: Tüm widgetlarý gizle
	void HideAllWidgets();
};