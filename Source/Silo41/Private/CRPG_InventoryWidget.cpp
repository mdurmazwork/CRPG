#include "CRPG_InventoryWidget.h"
#include "Components/UniformGridSlot.h"
#include "CRPG_HUD.h"
#include "Kismet/GameplayStatics.h"

void UCRPG_InventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Buton dinleyicisini baðla
	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &UCRPG_InventoryWidget::OnCloseClicked);
	}
}

void UCRPG_InventoryWidget::InitializeInventory(UCRPG_InventoryComponent* NewInventoryComp)
{
	if (!NewInventoryComp) return;

	LinkedInventory = NewInventoryComp;

	// Component üzerindeki "Güncelleme" olayýna abone ol
	// Böylece eþya toplayýnca ekran otomatik yenilenir.
	LinkedInventory->OnInventoryUpdated.RemoveDynamic(this, &UCRPG_InventoryWidget::RefreshInventory);
	LinkedInventory->OnInventoryUpdated.AddDynamic(this, &UCRPG_InventoryWidget::RefreshInventory);

	// Ýlk çizim
	RefreshInventory();
}

void UCRPG_InventoryWidget::RefreshInventory()
{
	if (!LinkedInventory || !InventoryGrid || !SlotClass) return;

	// 1. Izgarayý Temizle
	InventoryGrid->ClearChildren();

	// 2. Kapasite Yazýsý
	if (Txt_Capacity)
	{
		FString CapText = FString::Printf(TEXT("%d / %d"), LinkedInventory->InventoryContent.Num(), LinkedInventory->Capacity);
		Txt_Capacity->SetText(FText::FromString(CapText));
	}

	// 3. Slotlarý Oluþtur
	int32 ColCount = 4; // Her satýrda 4 eþya olsun

	for (int32 i = 0; i < LinkedInventory->InventoryContent.Num(); i++)
	{
		const FInventorySlot& SlotData = LinkedInventory->InventoryContent[i];

		// Slot Widget'ý yarat
		UUserWidget* SlotWidget = CreateWidget<UUserWidget>(this, SlotClass);
		if (SlotWidget)
		{
			// --- ZERO-BP LOGIC: DATA INJECTION ---
			// Slot Widget'ýn içinde "Img_Icon" ve "Txt_Count" olduðunu varsayýyoruz ve C++ ile buluyoruz.
			// Blueprint Node kullanmak yerine, Widget Tree'yi tarýyoruz.

			UImage* IconImage = Cast<UImage>(SlotWidget->GetWidgetFromName(TEXT("Img_Icon")));
			UTextBlock* CountText = Cast<UTextBlock>(SlotWidget->GetWidgetFromName(TEXT("Txt_Count")));

			if (IconImage && SlotData.ItemData)
			{
				IconImage->SetBrushFromTexture(SlotData.ItemData->Icon);
			}

			if (CountText)
			{
				// Miktar 1 ise sayýyý gizle, deðilse göster
				if (SlotData.Quantity > 1)
				{
					CountText->SetText(FText::AsNumber(SlotData.Quantity));
					CountText->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					CountText->SetVisibility(ESlateVisibility::Hidden);
				}
			}

			// Izgaraya yerleþtir (Matematik: Row = i / Col, Column = i % Col)
			UUniformGridSlot* GridSlot = InventoryGrid->AddChildToUniformGrid(SlotWidget, i / ColCount, i % ColCount);
			if (GridSlot)
			{
				GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			}
		}
	}
}

void UCRPG_InventoryWidget::OnCloseClicked()
{
	// HUD üzerinden kapatma isteði yolla
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