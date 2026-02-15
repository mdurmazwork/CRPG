#include "CRPG_InventoryWidget.h"
#include "Components/UniformGridSlot.h"
#include "CRPG_HUD.h"
#include "Kismet/GameplayStatics.h"
#include "CRPG_CharacterBase.h" 
#include "CRPG_AttributeComponent.h"

// ============================================================================
// IMPLEMENTATION: ITEM SLOT 
// ============================================================================

void UCRPG_ItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Slot)
	{
		Btn_Slot->OnClicked.RemoveDynamic(this, &UCRPG_ItemSlot::OnSlotClicked);
		Btn_Slot->OnClicked.AddDynamic(this, &UCRPG_ItemSlot::OnSlotClicked);
	}
}

void UCRPG_ItemSlot::InitSlot(UCRPG_ItemData* InItem, int32 Count, int32 Index, UCRPG_InventoryWidget* Parent)
{
	ItemData = InItem;
	IndexInInventory = Index;
	ParentInventory = Parent;

	// Buton her zaman aktif olsun
	if (Btn_Slot)
	{
		Btn_Slot->SetIsEnabled(true);
	}

	// 1. BOÞ SLOT
	if (ItemData == nullptr)
	{
		if (Img_Icon) Img_Icon->SetVisibility(ESlateVisibility::Hidden);

		if (Txt_Count)
		{
			Txt_Count->SetText(FText::GetEmpty());
			Txt_Count->SetVisibility(ESlateVisibility::Hidden);
		}

		return;
	}

	// 2. DOLU SLOT
	if (Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(ItemData->Icon);

		// [FIX 1] KRÝTÝK DÜZELTME: TIKLAMA SORUNU
		// ESlateVisibility::Visible yaparsak resim týklamayý YUTAR.
		// HitTestInvisible yaparsak resim görünür ama týklama ARKADAKÝ BUTONA geçer.
		Img_Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (Txt_Count)
	{
		if (Count > 1)
		{
			Txt_Count->SetText(FText::AsNumber(Count));
			// Yazý da týklamayý engellemesin
			Txt_Count->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Txt_Count->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UCRPG_ItemSlot::OnSlotClicked()
{
	if (ParentInventory)
	{
		if (ItemData)
		{
			UE_LOG(LogTemp, Log, TEXT("INVENTORY: Selected %s"), *ItemData->Name.ToString());
		}
		ParentInventory->SetSelectedItem(ItemData);
	}
}

// ============================================================================
// IMPLEMENTATION: INVENTORY WIDGET
// ============================================================================

void UCRPG_InventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &UCRPG_InventoryWidget::OnCloseClicked);
	}

	SetSelectedItem(nullptr);
}

void UCRPG_InventoryWidget::InitializeInventory(UCRPG_InventoryComponent* NewInventoryComp)
{
	if (!NewInventoryComp) return;

	LinkedInventory = NewInventoryComp;

	LinkedInventory->OnInventoryUpdated.RemoveDynamic(this, &UCRPG_InventoryWidget::RefreshInventory);
	LinkedInventory->OnInventoryUpdated.AddDynamic(this, &UCRPG_InventoryWidget::RefreshInventory);

	RefreshInventory();
	UpdateCharacterInfo();
}

void UCRPG_InventoryWidget::RefreshInventory()
{
	if (!LinkedInventory || !InventoryGrid || !SlotClass) return;

	InventoryGrid->SetSlotPadding(FMargin(5.0f));
	InventoryGrid->ClearChildren();

	// Kapasite Bilgisi
	if (Txt_Capacity)
	{
		int32 UsedSlots = 0;
		for (const auto& InvSlot : LinkedInventory->InventoryContent)
		{
			if (InvSlot.IsValid()) UsedSlots++;
		}

		int32 TotalSlots = LinkedInventory->Capacity;
		Txt_Capacity->SetText(FText::FromString(FString::Printf(TEXT("CAPACITY: %d / %d"), UsedSlots, TotalSlots)));
	}

	const TArray<FInventorySlot>& Content = LinkedInventory->InventoryContent;

	for (int32 i = 0; i < LinkedInventory->Capacity; i++)
	{
		UCRPG_ItemSlot* NewSlot = CreateWidget<UCRPG_ItemSlot>(this, SlotClass);
		if (!NewSlot) continue;

		if (Content.IsValidIndex(i))
		{
			NewSlot->InitSlot(Content[i].ItemData, Content[i].Quantity, i, this);
		}
		else
		{
			NewSlot->InitSlot(nullptr, 0, i, this);
		}

		int32 Row = i / Columns;
		int32 Col = i % Columns;

		UUniformGridSlot* GridSlot = InventoryGrid->AddChildToUniformGrid(NewSlot, Row, Col);
		if (GridSlot)
		{
			GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
	}
}

void UCRPG_InventoryWidget::UpdateCharacterInfo()
{
	if (!LinkedInventory) return;
	ACRPG_CharacterBase* OwnerChar = Cast<ACRPG_CharacterBase>(LinkedInventory->GetOwner());

	if (!OwnerChar) return;

	if (Img_CharPortrait && OwnerChar->MutationData && OwnerChar->MutationData->Portrait)
	{
		Img_CharPortrait->SetBrushFromTexture(OwnerChar->MutationData->Portrait);
	}

	if (OwnerChar->AttributeComp)
	{
		if (Txt_Stats_HP)
		{
			FString HPStr = FString::Printf(TEXT("HP: %.0f / %.0f"),
				OwnerChar->AttributeComp->GetHealthPercent() * 100.0f,
				100.0f);
			Txt_Stats_HP->SetText(FText::FromString(HPStr));
		}

		if (Txt_Stats_Speed)
		{
			FString SpdStr = FString::Printf(TEXT("SPEED: %.0f"), OwnerChar->AttributeComp->GetMovementSpeed());
			Txt_Stats_Speed->SetText(FText::FromString(SpdStr));
		}
	}
}

void UCRPG_InventoryWidget::SetSelectedItem(UCRPG_ItemData* Item)
{
	if (Item)
	{
		if (Img_SelectedItemIcon)
		{
			Img_SelectedItemIcon->SetBrushFromTexture(Item->Icon);
			Img_SelectedItemIcon->SetVisibility(ESlateVisibility::Visible); // Burasý sadece gösterim, týklanmaz, Visible kalabilir.
		}
		if (Txt_SelectedItemName) Txt_SelectedItemName->SetText(Item->Name);
		if (Txt_SelectedItemDesc) Txt_SelectedItemDesc->SetText(Item->Description);
	}
	else
	{
		if (Img_SelectedItemIcon) Img_SelectedItemIcon->SetVisibility(ESlateVisibility::Hidden);
		if (Txt_SelectedItemName) Txt_SelectedItemName->SetText(FText::FromString("Select an Item"));
		if (Txt_SelectedItemDesc) Txt_SelectedItemDesc->SetText(FText::GetEmpty());
	}
}

// [FIX 2] YENÝ: Arayüz Sýfýrlama
void UCRPG_InventoryWidget::ResetUI()
{
	SetSelectedItem(nullptr);
}

void UCRPG_InventoryWidget::OnCloseClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		ACRPG_HUD* HUD = Cast<ACRPG_HUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ToggleInventory();
		}
	}
}