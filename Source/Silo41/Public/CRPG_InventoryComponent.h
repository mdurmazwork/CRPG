#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CRPG_ItemData.h"
#include "CRPG_InventoryComponent.generated.h"

// Envanterdeki bir kutucuðu (Slot) temsil eder.
USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCRPG_ItemData* ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity;

	FInventorySlot()
	{
		ItemData = nullptr;
		Quantity = 0;
	}

	bool IsValid() const { return ItemData != nullptr && Quantity > 0; }
};

// Envanter deðiþtiðinde UI'ý uyarmak için Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SILO41_API UCRPG_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCRPG_InventoryComponent();

	// --- CONFIG ---

	// Maksimum Slot Sayýsý (Kapasite)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Inventory")
	int32 Capacity = 20;

	// Baþlangýç Eþyalarý (Editörden test için verilir)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Inventory")
	TArray<UCRPG_ItemData*> DefaultItems;

	// --- STATE ---

	// Çantanýn kendisi
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Silo41|Inventory")
	TArray<FInventorySlot> InventoryContent;

	// UI Güncelleme Sinyali
	UPROPERTY(BlueprintAssignable, Category = "Silo41|Events")
	FOnInventoryUpdated OnInventoryUpdated;

	// --- ACTIONS ---

	// Eþya Ekleme (Baþarýlý olursa true döner)
	UFUNCTION(BlueprintCallable, Category = "Silo41|Inventory")
	bool AddItem(UCRPG_ItemData* NewItem, int32 Count = 1);

	// Eþya Silme/Eksiltme
	UFUNCTION(BlueprintCallable, Category = "Silo41|Inventory")
	bool RemoveItem(UCRPG_ItemData* ItemToRemove, int32 Count = 1);

	// Belirli bir eþyadan kaç tane var?
	UFUNCTION(BlueprintCallable, Category = "Silo41|Inventory")
	int32 GetItemCount(FName ItemID);

	// Eþyayý kullan (FunctionTag'i tetikler)
	UFUNCTION(BlueprintCallable, Category = "Silo41|Inventory")
	bool UseItem(int32 SlotIndex);

protected:
	virtual void BeginPlay() override;
};