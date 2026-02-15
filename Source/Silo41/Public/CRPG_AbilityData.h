#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CRPG_AbilityData.generated.h"

// Yetenek Türü
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	Melee   UMETA(DisplayName = "Melee Attack"),
	Ranged  UMETA(DisplayName = "Ranged Attack"),
	Skill   UMETA(DisplayName = "Utility Skill") // Heal, Buff vs. 
};

// Hedef Tipi
UENUM(BlueprintType)
enum class ETargetType : uint8
{
	Enemy   UMETA(DisplayName = "Target Enemy"),
	Self    UMETA(DisplayName = "Target Self"),
	Ally    UMETA(DisplayName = "Target Ally"),
	Ground  UMETA(DisplayName = "Target Ground (AOE)")
};

/**
 * UCRPG_AbilityData
 * * YETENEK DNA'SI
 * Görevi: Bir yeteneðin ne olduðunu, ne kadara mal olduðunu ve ne yaptýðýný tanýmlar.
 * ActionComponent bu veriyi okuyarak yeteneði çalýþtýrýr.
 */
UCLASS(BlueprintType)
class SILO41_API UCRPG_AbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- KÝMLÝK (IDENTITY) ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	UTexture2D* Icon;

	// --- KURALLAR (RULES) ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	EAbilityType AbilityType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	ETargetType TargetType;

	// Bu yeteneði kullanmak kaç Action Point harcar?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	int32 APCost = 2;

	// Kaç turda bir kullanýlabilir? (0 = Her tur)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	int32 Cooldown = 0;

	// Yeteneðin menzili (cm). GridManager bunu Tile sayýsýna çevirebilir.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	float Range = 1000.0f;

	// --- ETKÝLER (EFFECTS) ---

	// Verilecek hasar miktarý (Negatif ise iyileþtirme yapabilir veya ayrý deðiþken tutulabilir)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	float DamageAmount = 10.0f;

	// Alan Etkisi Yarýçapý (0 ise tek hedef)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	float Radius = 0.0f;

	// --- GÖRSELLER (VISUALS) ---

	// Karakterin oynayacaðý animasyon (Örn: Ateþ etme, Kýlýç savurma)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UAnimMontage* CastAnimation;

	// Efekt (Örn: Namlu ateþi, Patlama, Kan efekti)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UParticleSystem* ImpactVFX; // veya UNiagaraSystem*

	// Ses Efekti
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	USoundBase* ImpactSound;

	// Asset Manager için ID Override
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AbilityData", GetFName());
	}
};