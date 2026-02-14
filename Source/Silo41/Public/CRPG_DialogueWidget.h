#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "CRPG_DialogueData.h"
#include "CRPG_DialogueWidget.generated.h"

// Forward Declaration (Ana sýnýfý butona tanýtmak için)
class UCRPG_DialogueWidget;

/**
 * UCRPG_OptionButton
 * * TEKÝL CEVAP BUTONU (ENTEGRE SINIF)
 * Görevi: Bir cevap þýkkýný temsil eder.
 * Not: Script limitini aþmamak için DialogueWidget içine gömülmüþtür.
 */
UCLASS()
class SILO41_API UCRPG_OptionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- BINDINGS ---
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Root;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Option;

	// --- SETUP ---
	void SetupOption(int32 NodeID, FText Text, UCRPG_DialogueWidget* Parent);

protected:
	virtual void NativeConstruct() override;

private:
	// Týklanýnca gidilecek ID
	int32 LinkedNodeID;

	// Ana Widget Referansý
	UPROPERTY()
	UCRPG_DialogueWidget* ParentDialogue;

	UFUNCTION()
	void OnButtonClicked();
};

/**
 * UCRPG_DialogueWidget
 * * DÝYALOG EKRANI
 * Görevi: Diyalog verisini okur ve entegre OptionButton sýnýfýný kullanarak þýklarý listeler.
 */
UCLASS()
class SILO41_API UCRPG_DialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- CONFIG ---

	// Þýk butonu sýnýfý (Entegre sýnýfý kullanýyoruz)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	TSubclassOf<UCRPG_OptionButton> OptionButtonClass;

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	float TypewriterSpeed = 0.02f;

	// --- BINDINGS ---

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_SpeakerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_DialogueBody;

	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UScrollBox* Scroll_History;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* OptionList;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Next;

	// --- LOGIC ---

	UFUNCTION(BlueprintCallable, Category = "Silo41|Dialogue")
	void StartDialogue(UCRPG_DialogueData* NewDialogue);

	// OptionButton tarafýndan çaðrýlýr (Public olmalý)
	void SelectOption(int32 NextNodeID);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCRPG_DialogueData* CurrentDialogueData;

	int32 CurrentNodeID;
	FString FullText;
	int32 CurrentCharIndex;
	FTimerHandle TimerHandle_Typewriter;

	void ShowNode(int32 NodeID);
	void OnTypewriterTick();
	void ShowOptions(const TArray<FDialogueOption>& Options);

	UFUNCTION()
	void OnNextClicked();
};