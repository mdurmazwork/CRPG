#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "CRPG_ActionComponent.h"
#include "CRPG_CombatWidget.generated.h"

/**
 * UCRPG_CombatWidget
 * * SAVAÞ ARAYÜZÜ
 * Görevi: Oyuncunun AP durumunu gösterir ve yeteneklerini (Skill Bar) listeler. 
 */
UCLASS()
class SILO41_API UCRPG_CombatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- CONFIG ---

	// Yetenek Butonu Tasarýmý (WBP_AbilityButton)
	// Ýçinde "Img_Icon" (Image) ve "Btn_Cast" (Button) olmalý.
	UPROPERTY(EditDefaultsOnly, Category = "Silo41|UI")
	TSubclassOf<UUserWidget> AbilityButtonClass;

	// --- BINDINGS (Görsel Elemanlar) ---

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_AP;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_EndTurn;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AbilityContainer; // Yeteneklerin dizileceði kutu

	// --- LOGIC ---

	// Savaþ baþladýðýnda veya sýra oyuncuya geldiðinde çaðrýlýr
	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void InitializeCombatHUD(UCRPG_ActionComponent* ActionComp);

	UFUNCTION(BlueprintCallable, Category = "Silo41|UI")
	void UpdateAP(int32 CurrentAP, int32 MaxAP);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCRPG_ActionComponent* LinkedActionComp;

	UFUNCTION()
	void OnEndTurnClicked();

	// Yetenek butonuna basýlýnca çalýþacak yardýmcý fonksiyon
	UFUNCTION()
	void OnAbilityClicked(UCRPG_AbilityData* Ability);
};