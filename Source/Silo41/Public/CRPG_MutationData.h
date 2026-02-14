#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CRPG_MutationData.generated.h"

USTRUCT(BlueprintType)
struct FMutationStats
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MovementSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ThermalResistance = 0.0f;
};

/**
 * UCRPG_MutationData
 * Eklenen Özellik: AggroRadius (Düþman Algý Mesafesi)
 */
UCLASS(BlueprintType)
class SILO41_API UCRPG_MutationData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Identity ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FText Description;

	// --- Visuals ---
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TSubclassOf<UAnimInstance> AnimClass;

	// --- Statistics ---
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	FMutationStats BaseStats;

	// --- AI / Combat Settings ---

	// Düþmanýn oyuncuyu görüp savaþý baþlatacaðý mesafe (cm).
	// Örn: 600 = 6 metre.
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	float AggroRadius = 600.0f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MutationData", GetFName());
	}
};