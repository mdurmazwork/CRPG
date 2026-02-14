#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "CRPG_DialogueData.h"
#include "CRPG_DialogueWidget.generated.h"

/**
 * UCRPG_DialogueWidget
 * * DÝYALOG EKRANI
 * Görevi: Diyalog verisini (DataAsset) okur, yazýyý daktilo efektiyle yazar
 * ve cevap butonlarýný (Options) listeler.
 */
UCLASS()
class SILO41_API UCRPG_DialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- CONFIG ---

	// Cevap þýkký için kullanýlacak Buton tasarýmý (WBP_OptionButton)
	// Bu Widget'ýn içinde tek bir TextBlock (Adý: "Txt_Option") olmalý.
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	TSubclassOf<UUserWidget> OptionButtonClass;

	// Harf yazma hýzý (Saniye)
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	float TypewriterSpeed = 0.03f;

	// --- BINDINGS (Görsel Elemanlar) ---

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_SpeakerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_DialogueBody;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* OptionList; // Butonlarýn dizileceði kutu

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Next; // Hýzlý geçmek için ekranýn tamamýný kaplayan görünmez buton

	// --- LOGIC ---

	UFUNCTION(BlueprintCallable, Category = "Silo41|Dialogue")
	void StartDialogue(UCRPG_DialogueData* NewDialogue);

protected:
	virtual void NativeConstruct() override;

private:
	// Aktif Diyalog Verisi
	UPROPERTY()
	UCRPG_DialogueData* CurrentDialogueData;

	// Þu anki düðüm (Node)
	int32 CurrentNodeID;

	// Daktilo Efekti Deðiþkenleri
	FString FullText;
	int32 CurrentCharIndex;
	FTimerHandle TimerHandle_Typewriter;

	void ShowNode(int32 NodeID);
	void OnTypewriterTick();

	// Daktilo bitince þýklarý gösterir
	void ShowOptions(const TArray<FDialogueOption>& Options);

	UFUNCTION()
	void OnNextClicked(); // Hýzlý geçme

	// Cevap þýkkýna týklandýðýnda (C++ tarafýnda dinamik delegate baðlama biraz karmaþýktýr,
	// bu yüzden basit bir "ID ile cevapla" fonksiyonu kullanacaðýz)
	UFUNCTION()
	void SelectOption(int32 NextNodeID);
};