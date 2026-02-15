#include "CRPG_InventoryComponent.h"
#include "CRPG_CharacterBase.h" 

UCRPG_InventoryComponent::UCRPG_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Capacity = 70; // 10x7 Grid istediðin için
}

void UCRPG_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// [ÖNEMLÝ] Grid sisteminin düzgün çalýþmasý için diziyi kapasite kadar "Boþ Slot" ile dolduruyoruz.
	InventoryContent.Empty();
	for (int32 i = 0; i < Capacity; i++)
	{
		InventoryContent.Add(FInventorySlot()); // Boþ struct ekle
	}

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
		for (int32 i = 0; i < InventoryContent.Num(); i++)
		{
			if (InventoryContent[i].ItemData == NewItem)
			{
				// Limit kontrolü (MaxStackSize) buraya eklenebilir
				InventoryContent[i].Quantity += Count;

				UE_LOG(LogTemp, Log, TEXT("INVENTORY: Stacked %d x %s at Index %d. Total: %d"), Count, *NewItem->Name.ToString(), i, InventoryContent[i].Quantity);
				OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}

	// 2. Boþ Bir Slot Bul (Add yapmak yerine boþ olaný dolduruyoruz)
	for (int32 i = 0; i < InventoryContent.Num(); i++)
	{
		if (!InventoryContent[i].IsValid()) // Slot boþ mu?
		{
			InventoryContent[i].ItemData = NewItem;
			InventoryContent[i].Quantity = Count;

			UE_LOG(LogTemp, Log, TEXT("INVENTORY: Added %s to Empty Slot %d"), *NewItem->Name.ToString(), i);
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("INVENTORY FULL: Could not add %s"), *NewItem->Name.ToString());
	return false;
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
				// [GÜNCELLEME] RemoveAt kullanmýyoruz! Slotu sýfýrlýyoruz.
				// Böylece diðer eþyalar kaymýyor.
				InventoryContent[i].ItemData = nullptr;
				InventoryContent[i].Quantity = 0;

				UE_LOG(LogTemp, Log, TEXT("INVENTORY: Cleared slot %d"), i);
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

	if (Slot.IsValid() && Slot.ItemData)
	{
		// Consumable mantýðý buraya gelecek
		UE_LOG(LogTemp, Log, TEXT("INVENTORY: Used %s from Slot %d"), *Slot.ItemData->Name.ToString(), SlotIndex);

		// Tüketilebilir ise 1 tane azalt
		if (Slot.ItemData->ItemType == EItemType::Consumable)
		{
			RemoveItem(Slot.ItemData, 1);
		}
		return true;
	}
	return false;
}