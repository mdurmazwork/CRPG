#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "CRPG_HUD.generated.h"

/**
 * ACRPG_HUD
 * * UI YÖNETÝCÝSÝ
 */
UCLASS()
class SILO41_API ACRPG_HUD : public AHUD
{
	GENERATED_BODY()

public:
	ACRPG_HUD();

	// --- CONFIG --- 
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> MainHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> CombatWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI Classes")
	TSubclassOf<UUserWidget> DialogueWidgetClass;

	// --- CONTROL API ---

	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void SwitchToExplorationMode();

	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void SwitchToCombatMode();

	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void ShowDialogue(bool bShow, AActor* TargetNPC = nullptr);

	UUserWidget* GetCombatWidget() const { return CombatWidgetInstance; }
	UUserWidget* GetInventoryWidget() const { return InventoryWidgetInstance; }
	UUserWidget* GetDialogueWidget() const { return DialogueWidgetInstance; }

	// Durum Kontrolleri
	bool IsDialogueActive() const;

	// [YENÝ] Envanter açýk mý kontrolü
	bool IsInventoryActive() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UUserWidget* MainHUDInstance;

	UPROPERTY()
	UUserWidget* InventoryWidgetInstance;

	UPROPERTY()
	UUserWidget* CombatWidgetInstance;

	UPROPERTY()
	UUserWidget* DialogueWidgetInstance;

	bool bIsDialogueVisible;

	void HideAllWidgets();
};