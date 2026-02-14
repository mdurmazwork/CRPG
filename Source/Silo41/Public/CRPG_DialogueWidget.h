#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "CRPG_DialogueData.h"
#include "CRPG_DialogueWidget.generated.h"

// Forward Declaration
class UCRPG_DialogueWidget;

/**
 * UCRPG_OptionButton
 * * TEKÝL CEVAP BUTONU
 * Görevi: ID'yi ve üzerindeki METNÝ saklar. Týklanýnca her ikisini de ana widget'a yollar.
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

	// Butonun üzerindeki metin (History'e eklemek için saklýyoruz)
	FText LinkedText;

	// Ana Widget Referansý
	UPROPERTY()
	UCRPG_DialogueWidget* ParentDialogue;

	UFUNCTION()
	void OnButtonClicked();
};

/**
 * UCRPG_DialogueWidget
 * * GELÝÞMÝÞ DÝYALOG SÝSTEMÝ (DISCO ELYSIUM STYLE)
 * Özellikler: Akan Tarihçe (History), Satýr Ýçi Ýsimler, Resume
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
	float TypewriterSpeed = 0.02f;

	// Geçmiþ konuþmalarýn rengi
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	FLinearColor HistoryTextColor = FLinearColor(0.7f, 0.7f, 0.7f, 0.6f);

	// Oyuncunun verdiði cevaplarýn rengi (History'de)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	FLinearColor PlayerHistoryColor = FLinearColor(0.4f, 0.7f, 1.0f, 0.8f);

	// Aktif konuþmacýnýn rengi (Yazýlan metin)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	FLinearColor ActiveTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// --- BINDINGS ---

	// [DEÐÝÞÝKLÝK] Txt_SpeakerName kaldýrýldý. Ýsim artýk metnin içinde.

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_DialogueBody;  // Aktif Metin (Ýsim dahil)

	UPROPERTY(meta = (BindWidget))
	UScrollBox* Scroll_History;    // Geçmiþ Metinler Kutusu

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* OptionList;      // Cevaplar

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Next;             // Hýzlý Geçiþ

	// --- LOGIC ---

	UFUNCTION(BlueprintCallable, Category = "Silo41|Dialogue")
	void StartDialogue(UCRPG_DialogueData* NewDialogue, int32 StartFromNodeID = -1);

	// OptionButton tarafýndan çaðrýlýr (Metni de alýr)
	void SelectOption(int32 NextNodeID, FText SelectedText);

	int32 GetCurrentNodeID() const { return CurrentNodeID; }

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCRPG_DialogueData* CurrentDialogueData;

	int32 CurrentNodeID;
	FString FullText; // Ýsim + Metin birleþmiþ hali
	int32 CurrentCharIndex;
	FTimerHandle TimerHandle_Typewriter;

	// Son konuþulanlarý hafýzaya atar
	void PushToHistory(const FString& FullMessage, bool bIsPlayer = false);

	void ShowNode(int32 NodeID);
	void OnTypewriterTick();
	void ShowOptions(const TArray<FDialogueOption>& Options);

	UFUNCTION()
	void OnNextClicked();
};