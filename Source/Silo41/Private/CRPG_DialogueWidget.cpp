#include "CRPG_DialogueWidget.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBoxSlot.h" 
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

void UCRPG_OptionButton::SetupOption(int32 NodeID, FText Text, UCRPG_DialogueWidget* Parent)
{
	LinkedNodeID = NodeID;
	LinkedText = Text;
	ParentDialogue = Parent;

	if (Txt_Option)
	{
		Txt_Option->SetText(Text);
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

void UCRPG_DialogueWidget::StartDialogue(UCRPG_DialogueData* NewDialogue, int32 StartFromNodeID)
{
	if (!NewDialogue) return;

	CurrentDialogueData = NewDialogue;

	// Yeni konuþma baþladýðýnda history'i temizle
	if (Scroll_History) Scroll_History->ClearChildren();

	int32 TargetID = (StartFromNodeID != -1) ? StartFromNodeID : NewDialogue->StartNodeID;
	ShowNode(TargetID);
}

void UCRPG_DialogueWidget::PushToHistory(const FString& FullMessage, bool bIsPlayer)
{
	if (!Scroll_History) return;

	UTextBlock* HistoryLine = NewObject<UTextBlock>(Scroll_History);

	if (HistoryLine)
	{
		// Metni ayarla (Zaten formatlanmýþ geliyor)
		HistoryLine->SetText(FText::FromString(FullMessage));

		// Stil Ayarlarý
		HistoryLine->SetColorAndOpacity(bIsPlayer ? PlayerHistoryColor : HistoryTextColor);
		HistoryLine->SetAutoWrapText(true);

		if (Txt_DialogueBody)
		{
			HistoryLine->SetFont(Txt_DialogueBody->GetFont());
		}

		UPanelSlot* PanelSlot = Scroll_History->AddChild(HistoryLine);
		if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(PanelSlot))
		{
			// Satýr aralarýna boþluk
			ScrollSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 15.0f));

			// ÖNEMLÝ: History'deki yazýlarýn saða/sola yaslanmasý burada ayarlanabilir.
			// Þimdilik hepsi sola yaslý.
			ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
		}

		// Her zaman en alta odaklan
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

	// [GEÇMÝÞE AT]
	// Eðer ekranda halihazýrda yazýlmýþ bir metin varsa, onu tarihçeye at.
	if (Txt_DialogueBody && !Txt_DialogueBody->GetText().IsEmpty())
	{
		PushToHistory(FullText, false); // false = NPC konuþtu
	}

	CurrentNodeID = NodeID;

	// [YENÝ MANTIK] Ýsim ve Metni Birleþtir
	FString Speaker = Node->SpeakerName.ToString().ToUpper();
	if (Speaker.IsEmpty()) Speaker = TEXT("UNKNOWN");

	// Format: "MARCUS: Buradan çýkmamýz lazým."
	FullText = FString::Printf(TEXT("%s: %s"), *Speaker, *Node->Text.ToString());

	CurrentCharIndex = 0;

	if (Txt_DialogueBody)
	{
		Txt_DialogueBody->SetText(FText::GetEmpty());
		Txt_DialogueBody->SetColorAndOpacity(ActiveTextColor);
	}

	if (OptionList) OptionList->ClearChildren();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Typewriter, this, &UCRPG_DialogueWidget::OnTypewriterTick, TypewriterSpeed, true);
}

void UCRPG_DialogueWidget::OnTypewriterTick()
{
	if (!Txt_DialogueBody)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);
		return;
	}

	CurrentCharIndex++;

	// Daktilo efekti
	FString SubString = FullText.Left(CurrentCharIndex);
	Txt_DialogueBody->SetText(FText::FromString(SubString));

	// ScrollBox'ý sürekli aþaðý it ki yazý akarken görünür kalsýn
	if (Scroll_History) Scroll_History->ScrollToEnd();

	if (CurrentCharIndex >= FullText.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);

		if (CurrentDialogueData)
		{
			FDialogueNode* Node = CurrentDialogueData->GetNodeByID(CurrentNodeID);
			if (Node) ShowOptions(Node->PlayerOptions);
		}
	}
}

void UCRPG_DialogueWidget::OnNextClicked()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Typewriter))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);
		if (Txt_DialogueBody) Txt_DialogueBody->SetText(FText::FromString(FullText));

		if (CurrentDialogueData)
		{
			FDialogueNode* Node = CurrentDialogueData->GetNodeByID(CurrentNodeID);
			if (Node) ShowOptions(Node->PlayerOptions);
		}

		if (Scroll_History) Scroll_History->ScrollToEnd();
	}
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
			EndBtn->SetupOption(-1, EndText, this);
			OptionList->AddChildToVerticalBox(EndBtn);
		}
		return;
	}

	for (const FDialogueOption& Option : Options)
	{
		UCRPG_OptionButton* BtnWidget = CreateWidget<UCRPG_OptionButton>(this, OptionButtonClass);
		if (BtnWidget)
		{
			BtnWidget->SetupOption(Option.NextNodeID, Option.Text, this);
			OptionList->AddChildToVerticalBox(BtnWidget);
		}
	}

	if (Scroll_History) Scroll_History->ScrollToEnd();
}

void UCRPG_DialogueWidget::SelectOption(int32 NextNodeID, FText SelectedText)
{
	// Oyuncunun seçtiði cevabý da tarihçeye ekle
	if (!SelectedText.IsEmpty())
	{
		// Format: "YOU: Cevap..."
		FString PlayerMsg = FString::Printf(TEXT("YOU: %s"), *SelectedText.ToString());
		PushToHistory(PlayerMsg, true); // true = Oyuncu
	}

	ShowNode(NextNodeID);
}