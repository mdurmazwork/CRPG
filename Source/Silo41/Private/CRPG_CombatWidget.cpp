#include "CRPG_CombatWidget.h"
#include "Kismet/GameplayStatics.h"
#include "CRPG_PlayerController.h"

void UCRPG_CombatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_EndTurn)
	{
		Btn_EndTurn->OnClicked.AddDynamic(this, &UCRPG_CombatWidget::OnEndTurnClicked);
	}
}

void UCRPG_CombatWidget::InitializeCombatHUD(UCRPG_ActionComponent* ActionComp)
{
	if (!ActionComp || !AbilityContainer || !AbilityButtonClass) return;

	LinkedActionComp = ActionComp;

	// 1. AP Güncelleme Delegate'ini baðla
	LinkedActionComp->OnActionPointsChanged.RemoveDynamic(this, &UCRPG_CombatWidget::UpdateAP);
	LinkedActionComp->OnActionPointsChanged.AddDynamic(this, &UCRPG_CombatWidget::UpdateAP);

	// Ýlk AP deðerini yaz
	UpdateAP(LinkedActionComp->CurrentActionPoints, LinkedActionComp->MaxActionPoints);

	// 2. Yetenek Butonlarýný Oluþtur
	AbilityContainer->ClearChildren();

	for (UCRPG_AbilityData* Ability : LinkedActionComp->DefaultAbilities)
	{
		if (!Ability) continue;

		UUserWidget* BtnWidget = CreateWidget<UUserWidget>(this, AbilityButtonClass);
		if (BtnWidget)
		{
			// Ýkonu Ayarla
			UImage* IconImg = Cast<UImage>(BtnWidget->GetWidgetFromName(TEXT("Img_Icon")));
			if (IconImg)
			{
				IconImg->SetBrushFromTexture(Ability->Icon);
			}

			// Týklama Olayý (Basit Yöntem: Butonun kendisine Ability bilgisini veremediðimiz için 
			// burada sadece görsel oluþturuyoruz. Gerçek bir yetenek sistemi için 
			// "CRPG_AbilityButton" C++ sýnýfý gerekir.
			// PROTOTÝP ÝÇÝN: Þimdilik sadece butonlarý diziyoruz.)

			// Not: C++ Widget içinde dinamik buton baðlamak karmaþýktýr.
			// Þimdilik sadece görsel olarak ekliyoruz.

			AbilityContainer->AddChildToHorizontalBox(BtnWidget);
		}
	}
}

void UCRPG_CombatWidget::UpdateAP(int32 CurrentAP, int32 MaxAP)
{
	if (Txt_AP)
	{
		FString APText = FString::Printf(TEXT("AP: %d / %d"), CurrentAP, MaxAP);
		Txt_AP->SetText(FText::FromString(APText));
	}
}

void UCRPG_CombatWidget::OnEndTurnClicked()
{
	// PlayerController'a "Turu Bitir" sinyali gönder
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// Input Action sistemini tetiklemek yerine doðrudan fonksiyon çaðýrabiliriz
		// Veya simüle edebiliriz. En temizi Controller'a public fonksiyon eklemektir.

		// Geçici Çözüm: Input sistemine býrakalým veya Controller cast edelim
		if (ACRPG_PlayerController* SiloPC = Cast<ACRPG_PlayerController>(PC))
		{
			// Input Action ile baðlý olan fonksiyonu public yapýp buradan çaðýrabilirsin
			// Þimdilik sadece Log atalým, çünkü Controller fonksiyonu private/protected olabilir.
			UE_LOG(LogTemp, Warning, TEXT("UI: End Turn Clicked (Connect to Controller Logic)"));
		}
	}
}

void UCRPG_CombatWidget::OnAbilityClicked(UCRPG_AbilityData* Ability)
{
	// Bu fonksiyonu "WBP_AbilityButton" içinden çaðýrmak gerekecek.
	UE_LOG(LogTemp, Log, TEXT("UI: Ability Selected: %s"), *Ability->AbilityName.ToString());
}