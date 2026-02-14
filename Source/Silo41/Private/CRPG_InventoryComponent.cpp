#include "CRPG_InventoryComponent.h"
#include "CRPG_CharacterBase.h" // Ýleride karakterin fonksiyonunu çaðýrmak için

UCRPG_InventoryComponent::UCRPG_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Capacity = 20;
}

void UCRPG_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Baþlangýç eþyalarýný ekle
	for (UCRPG_ItemData* Item : DefaultItems)
	{
		AddItem(Item, 1);
	}
}

bool UCRPG_InventoryComponent::AddItem(UCRPG_ItemData* NewItem, int32 Count)
{
	if (!NewItem || Count <= 0) return false;

	// 1. Eþya Yýðýnlanabilir (Stackable) ise, mevcut slotu bul
	if (NewItem->bIsStackable)
	{
		for (FInventorySlot& Slot : InventoryContent)
		{
			if (Slot.ItemData == NewItem)
			{
				// Limit kontrolü eklenebilir (MaxStackSize)
				Slot.Quantity += Count;

				UE_LOG(LogTemp, Log, TEXT("INVENTORY: Stacked %d x %s. Total: %d"), Count, *NewItem->Name.ToString(), Slot.Quantity);
				OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// 2. Yýðýnlanamaz veya çantada yoksa: Yeni Slot aç
	if (InventoryContent.Num() >= Capacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("INVENTORY FULL: Cannot add %s"), *NewItem->Name.ToString());
		return false;
	}

	FInventorySlot NewSlot;
	NewSlot.ItemData = NewItem;
	NewSlot.Quantity = Count;

	InventoryContent.Add(NewSlot);

	UE_LOG(LogTemp, Log, TEXT("INVENTORY: Added New Slot %d x %s"), Count, *NewItem->Name.ToString());
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UCRPG_InventoryComponent::RemoveItem(UCRPG_ItemData* ItemToRemove, int32 Count)
{
	if (!ItemToRemove || Count <= 0) return false;

	for (int32 i = 0; i < InventoryContent.Num(); i++)
	{
		if (InventoryContent[i].ItemData == ItemToRemove)
		{
			InventoryContent[i].Quantity -= Count;

			if (InventoryContent[i].Quantity <= 0)
			{
				InventoryContent.RemoveAt(i);
				UE_LOG(LogTemp, Log, TEXT("INVENTORY: Removed item slot %s"), *ItemToRemove->Name.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("INVENTORY: Decreased %s. New Qty: %d"), *ItemToRemove->Name.ToString(), InventoryContent[i].Quantity);
			}

			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	return false;
}

int32 UCRPG_InventoryComponent::GetItemCount(FName ItemID)
{
	int32 Total = 0;
	for (const FInventorySlot& Slot : InventoryContent)
	{
		if (Slot.ItemData && Slot.ItemData->ItemID == ItemID)
		{
			Total += Slot.Quantity;
		}
	}
	return Total;
}

bool UCRPG_InventoryComponent::UseItem(int32 SlotIndex)
{
	if (!InventoryContent.IsValidIndex(SlotIndex)) return false;

	FInventorySlot& Slot = InventoryContent[SlotIndex];
	if (!Slot.IsValid()) return false;

	UCRPG_ItemData* Item = Slot.ItemData;

	UE_LOG(LogTemp, Warning, TEXT("INVENTORY ACTION: Using %s (Tag: %s)"), *Item->Name.ToString(), *Item->FunctionTag.ToString());

	// TODO: Ýleride FunctionTag'e göre iþlem yapacaðýz.
	// Örn: if(Tag == "Heal_50") Player->Heal(50);

	// Tüketilebilir ise 1 adet eksilt
	if (Item->ItemType == EItemType::Consumable)
	{
		RemoveItem(Item, 1);
	}

	return true;
}