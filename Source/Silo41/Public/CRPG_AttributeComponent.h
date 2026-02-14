#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CRPG_MutationData.h"
#include "CRPG_StatusEffect.h" // Status Effect eriþimi
#include "CRPG_AttributeComponent.generated.h"

// Aktif bir etkinin çalýþma zamaný (Runtime) verisi
USTRUCT(BlueprintType)
struct FActiveStatusEffect
{
	GENERATED_BODY()

	// Etkinin kaynak verisi (DataAsset)
	UPROPERTY()
	UCRPG_StatusEffect* EffectData;

	// Kalan tur sayýsý
	UPROPERTY()
	int32 TurnsLeft;

	// Görsel efekt referansý (Silerken yok etmek için)
	UPROPERTY()
	UParticleSystemComponent* VFXInstance;

	// Operatörler (Array aramalarý için)
	bool operator==(const FActiveStatusEffect& Other) const
	{
		return EffectData == Other.EffectData;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SILO41_API UCRPG_AttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCRPG_AttributeComponent();

	void InitializeStats(const FMutationStats& BaseStats);

	// --- Health Operations ---
	UFUNCTION(BlueprintCallable, Category = "Silo41|Attributes")
	void ApplyHealthChange(float Delta);

	UFUNCTION(BlueprintPure, Category = "Silo41|Attributes")
	float GetHealthPercent() const;

	// --- Status Effect Operations (YENÝ) ---

	// Karaktere yeni bir etki uygular (Zehirle, Dondur vs.)
	UFUNCTION(BlueprintCallable, Category = "Silo41|Status")
	void AddStatusEffect(UCRPG_StatusEffect* NewEffect);

	// Tur baþýnda çaðrýlýr: DoT hasarlarýný uygular ve süreyi azaltýr.
	// Eðer karakter Stun yemiþse "TRUE" döner.
	bool ProcessTurnStart();

	// --- Getters ---
	float GetMovementSpeed() const; // Artýk dinamik hesaplanýyor (Slow etkisi varsa düþer)

protected:
	virtual void BeginPlay() override;

private:
	// Runtime Stats
	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float CurrentStamina;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float BaseMovementSpeed;

	// Aktif Etkiler Listesi
	UPROPERTY(VisibleAnywhere, Category = "Attributes|Status")
	TArray<FActiveStatusEffect> ActiveEffects;
};