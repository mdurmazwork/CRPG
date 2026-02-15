#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CRPG_InventoryComponent.h"
#include "CRPG_ItemData.h"
#include "CRPG_InventoryWidget.generated.h"

// ============================================================================
// 1. CLASS: ITEM SLOT (KUTUCUK)
// ============================================================================

class UCRPG_InventoryWidget; // Forward Declaration

UCLASS()
class SILO41_API UCRPG_ItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- BINDINGS ---
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Count;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Slot;

	// --- DATA ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Silo41")
	UCRPG_ItemData* ItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Silo41")
	int32 IndexInInventory;

	UPROPERTY()
	UCRPG_InventoryWidget* ParentInventory;

	void InitSlot(UCRPG_ItemData* InItem, int32 Count, int32 Index, UCRPG_InventoryWidget* Parent);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnSlotClicked();
};

// ============================================================================
// 2. CLASS: INVENTORY WIDGET (ANA EKRAN)
// ============================================================================

UCLASS()
class SILO41_API UCRPG_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	TSubclassOf<UCRPG_ItemSlot> SlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	int32 Columns = 10;

	// --- BINDINGS: LEFT SIDE (CHARACTER) ---
	UPROPERTY(meta = (BindWidget))
	UImage* Img_CharPortrait;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Stats_HP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Stats_Stamina;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Stats_Speed;

	// --- BINDINGS: RIGHT SIDE (DETAILS & GRID) ---
	UPROPERTY(meta = (BindWidget))
	UImage* Img_SelectedItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_SelectedItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_SelectedItemDesc;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Close;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Capacity;

	// --- FUNCTIONS ---
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void InitializeInventory(UCRPG_InventoryComponent* NewInventoryComp);

	UFUNCTION()
	void RefreshInventory();

	void SetSelectedItem(UCRPG_ItemData* Item);

	// [YENÝ] Arayüzü sýfýrlar (Seçili item'ý unutur)
	void ResetUI();

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCRPG_InventoryComponent* LinkedInventory;

	UFUNCTION()
	void OnCloseClicked();

	void UpdateCharacterInfo();
};