#include "CRPG_DialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBoxSlot.h" 
#include "Components/Image.h"
#include "CRPG_HUD.h"
#include "Kismet/GameplayStatics.h"

// ============================================================================
// UCRPG_OptionButton IMPLEMENTATION 
// ============================================================================

void UCRPG_OptionButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Root)
	{
		Btn_Root->OnClicked.RemoveDynamic(this, &UCRPG_OptionButton::OnButtonClicked);
		Btn_Root->OnClicked.AddDynamic(this, &UCRPG_OptionButton::OnButtonClicked);
	}
}

void UCRPG_OptionButton::SetupOption(int32 NodeID, FText OriginalText, int32 Index, UCRPG_DialogueWidget* Parent)
{
	LinkedNodeID = NodeID;
	LinkedText = OriginalText;
	ParentDialogue = Parent;

	if (Txt_Option)
	{
		FString NumberedStr = FString::Printf(TEXT("%d. - \"%s\""), Index, *OriginalText.ToString());
		Txt_Option->SetText(FText::FromString(NumberedStr));
	}
}

void UCRPG_OptionButton::OnButtonClicked()
{
	if (ParentDialogue)
	{
		ParentDialogue->SelectOption(LinkedNodeID, LinkedText);
	}
}

// ============================================================================
// UCRPG_DialogueWidget IMPLEMENTATION
// ============================================================================

void UCRPG_DialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Next)
	{
		Btn_Next->OnClicked.AddDynamic(this, &UCRPG_DialogueWidget::OnNextClicked);
	}

	if (Txt_DialogueBody) Txt_DialogueBody->SetText(FText::GetEmpty());
	if (OptionList) OptionList->ClearChildren();
	if (Scroll_History) Scroll_History->ClearChildren();
}

// [GÜNCELLENDÝ] Portre parametresi iþleniyor
void UCRPG_DialogueWidget::StartDialogue(UCRPG_DialogueData* NewDialogue, UTexture2D* PortraitTexture, int32 StartFromNodeID)
{
	if (!NewDialogue) return;

	CurrentDialogueData = NewDialogue;

	if (Scroll_History) Scroll_History->ClearChildren();

	// Portre Ayarý
	if (Img_SpeakerPortrait)
	{
		if (PortraitTexture)
		{
			Img_SpeakerPortrait->SetBrushFromTexture(PortraitTexture);
			Img_SpeakerPortrait->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// Portre yoksa gizle (yer kaplamasýn istiyorsan Collapsed, boþluk kalsýn istiyorsan Hidden)
			Img_SpeakerPortrait->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	int32 TargetID = (StartFromNodeID != -1) ? StartFromNodeID : NewDialogue->StartNodeID;
	ShowNode(TargetID);
}

void UCRPG_DialogueWidget::PushToHistory(const FString& FullMessage, bool bIsPlayer)
{
	if (!Scroll_History) return;

	UTextBlock* HistoryLine = NewObject<UTextBlock>(Scroll_History);

	if (HistoryLine)
	{
		HistoryLine->SetText(FText::FromString(FullMessage));
		HistoryLine->SetColorAndOpacity(bIsPlayer ? PlayerHistoryColor : HistoryTextColor);
		HistoryLine->SetAutoWrapText(true);

		if (Txt_DialogueBody)
		{
			HistoryLine->SetFont(Txt_DialogueBody->GetFont());
		}

		UPanelSlot* PanelSlot = Scroll_History->AddChild(HistoryLine);
		if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(PanelSlot))
		{
			ScrollSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 15.0f));
			ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
		}

		Scroll_History->ScrollToEnd();
	}
}

void UCRPG_DialogueWidget::ShowNode(int32 NodeID)
{
	if (NodeID == -1 || !CurrentDialogueData)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			ACRPG_HUD* HUD = Cast<ACRPG_HUD>(PC->GetHUD());
			if (HUD) HUD->ShowDialogue(false);
		}
		return;
	}

	FDialogueNode* Node = CurrentDialogueData->GetNodeByID(NodeID);
	if (!Node)
	{
		SelectOption(-1, FText::GetEmpty());
		return;
	}

	CurrentNodeID = NodeID;

	FString Speaker = Node->SpeakerName.ToString().ToUpper();
	if (Speaker.IsEmpty()) Speaker = TEXT("UNKNOWN");

	FullText = FString::Printf(TEXT("%s: \"%s\""), *Speaker, *Node->Text.ToString());

	if (Txt_DialogueBody)
	{
		Txt_DialogueBody->SetText(FText::FromString(FullText));
		Txt_DialogueBody->SetColorAndOpacity(ActiveTextColor);
	}

	if (OptionList) OptionList->ClearChildren();
	ShowOptions(Node->PlayerOptions);

	if (Scroll_History) Scroll_History->ScrollToEnd();
}

void UCRPG_DialogueWidget::OnNextClicked()
{
	if (Scroll_History) Scroll_History->ScrollToEnd();
}

void UCRPG_DialogueWidget::ShowOptions(const TArray<FDialogueOption>& Options)
{
	if (!OptionList || !OptionButtonClass) return;

	OptionList->ClearChildren();

	if (Options.Num() == 0)
	{
		UCRPG_OptionButton* EndBtn = CreateWidget<UCRPG_OptionButton>(this, OptionButtonClass);
		if (EndBtn)
		{
			FText EndText = FText::FromString("[End Dialogue]");
			EndBtn->SetupOption(-1, EndText, 1, this);
			OptionList->AddChildToVerticalBox(EndBtn);
		}
		return;
	}

	for (int32 i = 0; i < Options.Num(); i++)
	{
		const FDialogueOption& Option = Options[i];

		UCRPG_OptionButton* BtnWidget = CreateWidget<UCRPG_OptionButton>(this, OptionButtonClass);
		if (BtnWidget)
		{
			BtnWidget->SetupOption(Option.NextNodeID, Option.Text, i + 1, this);
			OptionList->AddChildToVerticalBox(BtnWidget);
		}
	}

	if (Scroll_History) Scroll_History->ScrollToEnd();
}

void UCRPG_DialogueWidget::SelectOption(int32 NextNodeID, FText SelectedText)
{
	if (!FullText.IsEmpty())
	{
		PushToHistory(FullText, false);
	}

	if (!SelectedText.IsEmpty())
	{
		FString PlayerMsg = FString::Printf(TEXT("YOU: \"%s\""), *SelectedText.ToString());
		PushToHistory(PlayerMsg, true);
	}

	ShowNode(NextNodeID);
}