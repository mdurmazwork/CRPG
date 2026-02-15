#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h" // Resim için gerekli
#include "CRPG_DialogueData.h"
#include "CRPG_DialogueWidget.generated.h"

// Forward Declaration
class UCRPG_DialogueWidget;

/**
 * UCRPG_OptionButton
 * * TEKÝL CEVAP BUTONU
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
	void SetupOption(int32 NodeID, FText OriginalText, int32 Index, UCRPG_DialogueWidget* Parent);

protected:
	virtual void NativeConstruct() override;

private:
	int32 LinkedNodeID;
	FText LinkedText;

	UPROPERTY()
	UCRPG_DialogueWidget* ParentDialogue;

	UFUNCTION()
	void OnButtonClicked();
};

/**
 * UCRPG_DialogueWidget
 * * GELÝÞMÝÞ DÝYALOG SÝSTEMÝ
 * Özellik: Portre desteði eklendi.
 */
UCLASS()
class SILO41_API UCRPG_DialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- CONFIG ---
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	TSubclassOf<UCRPG_OptionButton> OptionButtonClass;

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	FLinearColor HistoryTextColor = FLinearColor(0.7f, 0.7f, 0.7f, 0.6f);

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	FLinearColor PlayerHistoryColor = FLinearColor(0.4f, 0.7f, 1.0f, 0.8f);

	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	FLinearColor ActiveTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// --- BINDINGS ---

	// [YENÝ] Konuþmacý Portresi
	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UImage* Img_SpeakerPortrait;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_DialogueBody;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* Scroll_History;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* OptionList;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Next;

	// --- LOGIC ---

	// [GÜNCELLENDÝ] Artýk opsiyonel olarak bir portre texture'ý alýyor
	UFUNCTION(BlueprintCallable, Category = "Silo41|Dialogue")
	void StartDialogue(UCRPG_DialogueData* NewDialogue, UTexture2D* PortraitTexture = nullptr, int32 StartFromNodeID = -1);

	void SelectOption(int32 NextNodeID, FText SelectedText);
	int32 GetCurrentNodeID() const { return CurrentNodeID; }

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCRPG_DialogueData* CurrentDialogueData;

	int32 CurrentNodeID;
	FString FullText;

	void PushToHistory(const FString& FullMessage, bool bIsPlayer = false);
	void ShowNode(int32 NodeID);
	void ShowOptions(const TArray<FDialogueOption>& Options);

	UFUNCTION()
	void OnNextClicked();
};