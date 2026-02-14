#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CRPG_InventoryComponent.h"
#include "CRPG_InventoryWidget.generated.h"

/**
 * ACRPG_InventoryWidget
 * * ENVANTER EKRANI
 * Görevi: InventoryComponent verisini okur ve ekrana "Kutu Kutu" (Grid) basar.
 * Zero-BP Kuralý: Slot'larýn içinin doldurulmasý (Ýkon, Sayý) tamamen burada yapýlýr.
 */
UCLASS()
class SILO41_API UCRPG_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- CONFIG ---

	// Izgaranýn içine koyacaðýmýz "Kutucuk" tasarýmý (WBP_ItemSlot)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	TSubclassOf<UUserWidget> SlotClass;

	// --- BINDINGS (Görsel Elemanlar) ---
	// WBP tarafýnda bu isimler BÝREBÝR ayný olmalý!

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Close;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Capacity;

	// --- FUNCTIONS ---

	// Envanter Component'ini bu Widget'a tanýtan fonksiyon
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void InitializeInventory(UCRPG_InventoryComponent* NewInventoryComp);

	// Ekraný temizle ve yeniden çiz
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void RefreshInventory();

protected:
	virtual void NativeConstruct() override;

private:
	// Hangi component'i dinliyoruz?
	UPROPERTY()
	UCRPG_InventoryComponent* LinkedInventory;

	UFUNCTION()
	void OnCloseClicked();
};