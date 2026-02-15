#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CRPG_StatusEffect.generated.h"

// Etki Türü
UENUM(BlueprintType)
enum class EStatusType : uint8
{
	Debuff  UMETA(DisplayName = "Harmful Effect"), // Zehir, Kanama
	Buff    UMETA(DisplayName = "Beneficial Effect"), // Ýyileþme, Hýzlanma
	Control UMETA(DisplayName = "Crowd Control") // Sersemletme, Dondurma
};

/**
 * UCRPG_StatusEffect
 * * ETKÝ TANIMI
 * Görevi: Bir durum etkisinin "Kurallarýný" tutar.
 * AttributeComponent bu veriyi okuyarak her tur iþlem yapar. 
 */
UCLASS(BlueprintType)
class SILO41_API UCRPG_StatusEffect : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- KÝMLÝK ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	EStatusType Type;

	// --- KURALLAR ---

	// Etki kaç tur sürecek?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	int32 DurationTurns = 3;

	// Her tur baþýnda verilecek hasar (Negatif ise iyileþtirir)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	float DamagePerTurn = 0.0f;

	// Hareket hýzý çarpaný (0.5 = Yarý hýz, 0.0 = Hareketsiz, 1.0 = Normal)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	float SpeedMultiplier = 1.0f;

	// Eðer true ise, karakter sýrasýný pas geçer (Stun/Freeze)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	bool bStunsCharacter = false;

	// --- GÖRSELLER ---

	// Karakterin üzerinde sürekli oynayacak efekt (Örn: Yanma alevi)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UParticleSystem* PersistentVFX;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("StatusEffect", GetFName());
	}
};