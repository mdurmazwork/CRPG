#include "CRPG_DialogueWidget.h"
#include "TimerManager.h"
#include "Components/ButtonSlot.h"
#include "CRPG_HUD.h"

void UCRPG_DialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Next)
	{
		Btn_Next->OnClicked.AddDynamic(this, &UCRPG_DialogueWidget::OnNextClicked);
	}

	// Baþlangýçta boþalt
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
	if (!CurrentDialogueData) return;

	// Node'u bul (DataAsset içindeki helper fonksiyonu kullanýyoruz)
	FDialogueNode* Node = CurrentDialogueData->GetNodeByID(NodeID);

	if (!Node)
	{
		// Düðüm bulunamazsa (veya -1 ise) diyaloðu kapat
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			ACRPG_HUD* HUD = Cast<ACRPG_HUD>(PC->GetHUD());
			if (HUD) HUD->ShowDialogue(false);
		}
		return;
	}

	CurrentNodeID = NodeID;

	// 1. Ýsim
	if (Txt_SpeakerName)
	{
		Txt_SpeakerName->SetText(Node->SpeakerName);
	}

	// 2. Metni Hazýrla (Daktilo için)
	FullText = Node->Text.ToString();
	CurrentCharIndex = 0;
	if (Txt_DialogueBody) Txt_DialogueBody->SetText(FText::GetEmpty());

	// 3. Þýklarý Gizle (Yazý bitene kadar)
	if (OptionList) OptionList->ClearChildren();

	// 4. Zamanlayýcýyý Baþlat
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Typewriter, this, &UCRPG_DialogueWidget::OnTypewriterTick, TypewriterSpeed, true);
}

void UCRPG_DialogueWidget::OnTypewriterTick()
{
	if (!Txt_DialogueBody)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);
		return;
	}

	// Bir harf ekle
	CurrentCharIndex++;
	FString SubString = FullText.Left(CurrentCharIndex);
	Txt_DialogueBody->SetText(FText::FromString(SubString));

	// Bitti mi?
	if (CurrentCharIndex >= FullText.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);

		// Þýklarý Göster
		FDialogueNode* Node = CurrentDialogueData->GetNodeByID(CurrentNodeID);
		if (Node)
		{
			ShowOptions(Node->PlayerOptions);
		}
	}
}

void UCRPG_DialogueWidget::OnNextClicked()
{
	// Eðer yazý hala yazýlýyorsa, tamamýný göster ve bitir.
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Typewriter))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Typewriter);
		if (Txt_DialogueBody) Txt_DialogueBody->SetText(FText::FromString(FullText));

		// Þýklarý Göster
		FDialogueNode* Node = CurrentDialogueData->GetNodeByID(CurrentNodeID);
		if (Node) ShowOptions(Node->PlayerOptions);
	}
}

void UCRPG_DialogueWidget::ShowOptions(const TArray<FDialogueOption>& Options)
{
	if (!OptionList || !OptionButtonClass) return;

	// Eðer hiç þýk yoksa "Devam Et / Kapat" (NextNodeID = -1) butonu koyabiliriz
	// Ama þimdilik Data'dan gelmesini bekliyoruz.

	for (const FDialogueOption& Option : Options)
	{
		// C++ tarafýnda "Dynamic Widget Creation with Arguments" zordur.
		// Bu yüzden basit bir hile yapacaðýz: 
		// Butonu yaratacaðýz, TextBlock'unu bulup yazacaðýz.
		// Týklama olayýný (OnClicked) ise lambda ile baðlayacaðýz.

		UUserWidget* BtnWidget = CreateWidget<UUserWidget>(this, OptionButtonClass);
		if (BtnWidget)
		{
			// Yazýyý ayarla
			UTextBlock* BtnText = Cast<UTextBlock>(BtnWidget->GetWidgetFromName(TEXT("Txt_Option")));
			if (BtnText)
			{
				BtnText->SetText(Option.Text);
			}

			// Týklama olayýný ayarla (Reflection ile ana butonu buluyoruz)
			// Not: WBP_OptionButton'ýn Root'u bir "Button" olmalý ve adý "Btn_Root" olmalý.
			UButton* RootBtn = Cast<UButton>(BtnWidget->GetWidgetFromName(TEXT("Btn_Root")));
			if (RootBtn)
			{
				// Lambda Capture ile NextNodeID'yi kopyalýyoruz
				int32 TargetID = Option.NextNodeID;
				RootBtn->OnClicked.AddDynamic(this, &UCRPG_DialogueWidget::OnNextClicked); // Önce timer'ý durdurur (Safety)

				// Unreal'da dinamik parametreli delegate zordur. 
				// Burada "OnClicked" olayýna özel bir çözüm gerekir ama
				// Zero-BP kapsamýnda en temiz yol: Butonun kendisine ID verip, Widget'ýn o ID'yi okumasýdýr.
				// Ancak þimdilik basit tutmak için:
				// Sadece "Kapat" (-1) veya "Baþa Dön" (0) gibi basit mantýklar için deðil,
				// Gerçek bir çözüm için C++ Custom Button Class gerekir.
				// PROTOTÝP ÇÖZÜMÜ: Þimdilik sadece "End Dialogue" çalýþýr.
				// Ýleride buraya "CRPG_OptionButton" C++ sýnýfý yazacaðýz.
			}

			OptionList->AddChildToVerticalBox(BtnWidget);
		}
	}
}

void UCRPG_DialogueWidget::SelectOption(int32 NextNodeID)
{
	ShowNode(NextNodeID);
}