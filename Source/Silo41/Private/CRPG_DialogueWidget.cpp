#include "CRPG_DialogueWidget.h"
#include "TimerManager.h"
#include "CRPG_HUD.h"

// ============================================================================
// UCRPG_OptionButton IMPLEMENTATION (ENTEGRE BUTON SINIFI)
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
		ParentDialogue->SelectOption(LinkedNodeID);
	}
}

// ============================================================================
// UCRPG_DialogueWidget IMPLEMENTATION (ANA DÝYALOG PENCERESÝ)
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
}

void UCRPG_DialogueWidget::StartDialogue(UCRPG_DialogueData* NewDialogue)
{
	if (!NewDialogue) return;

	CurrentDialogueData = NewDialogue;
	ShowNode(NewDialogue->StartNodeID);
}

void UCRPG_DialogueWidget::ShowNode(int32 NodeID)
{
	// -1 gelirse veya data yoksa kapat
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
		// Hatalý ID gelirse de kapat
		UE_LOG(LogTemp, Error, TEXT("DIALOGUE ERROR: Node ID %d not found!"), NodeID);
		SelectOption(-1);
		return;
	}

	CurrentNodeID = NodeID;

	// Ýsim
	if (Txt_SpeakerName) Txt_SpeakerName->SetText(Node->SpeakerName);

	// Metin Hazýrlýðý
	FullText = Node->Text.ToString();
	CurrentCharIndex = 0;
	if (Txt_DialogueBody) Txt_DialogueBody->SetText(FText::GetEmpty());

	// Þýklarý temizle (Yazý bitene kadar gösterme)
	if (OptionList) OptionList->ClearChildren();

	// Timer Baþlat
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
	FString SubString = FullText.Left(CurrentCharIndex);
	Txt_DialogueBody->SetText(FText::FromString(SubString));

	if (Scroll_History) Scroll_History->ScrollToEnd();

	if (CurrentCharIndex >= FullText.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);

		// Yazý bitti, þýklarý dök
		if (CurrentDialogueData)
		{
			FDialogueNode* Node = CurrentDialogueData->GetNodeByID(CurrentNodeID);
			if (Node) ShowOptions(Node->PlayerOptions);
		}
	}
}

void UCRPG_DialogueWidget::OnNextClicked()
{
	// Hýzlý geçiþ
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Typewriter))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);
		if (Txt_DialogueBody) Txt_DialogueBody->SetText(FText::FromString(FullText));

		if (CurrentDialogueData)
		{
			FDialogueNode* Node = CurrentDialogueData->GetNodeByID(CurrentNodeID);
			if (Node) ShowOptions(Node->PlayerOptions);
		}
	}
}

void UCRPG_DialogueWidget::ShowOptions(const TArray<FDialogueOption>& Options)
{
	if (!OptionList || !OptionButtonClass) return;

	OptionList->ClearChildren();

	// Eðer seçenek yoksa, otomatik "Kapat" butonu koy
	if (Options.Num() == 0)
	{
		UCRPG_OptionButton* EndBtn = CreateWidget<UCRPG_OptionButton>(this, OptionButtonClass);
		if (EndBtn)
		{
			EndBtn->SetupOption(-1, FText::FromString("[End Dialogue]"), this);
			OptionList->AddChildToVerticalBox(EndBtn);
		}
		return;
	}

	for (const FDialogueOption& Option : Options)
	{
		// Entegre C++ sýnýfýný kullanarak butonu yarat
		UCRPG_OptionButton* BtnWidget = CreateWidget<UCRPG_OptionButton>(this, OptionButtonClass);

		if (BtnWidget)
		{
			BtnWidget->SetupOption(Option.NextNodeID, Option.Text, this);
			OptionList->AddChildToVerticalBox(BtnWidget);
		}
	}
}

void UCRPG_DialogueWidget::SelectOption(int32 NextNodeID)
{
	UE_LOG(LogTemp, Log, TEXT("DIALOGUE: Option Selected -> Going to Node %d"), NextNodeID);
	ShowNode(NextNodeID);
}