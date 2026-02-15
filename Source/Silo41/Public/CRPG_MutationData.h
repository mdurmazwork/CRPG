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
 * Eklenen Özellik: Portrait (Diyalog Portresi)
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

	// [YENÝ] Karakterin Diyalog Ekranýndaki Portresi
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	UTexture2D* Portrait;

	// --- Visuals ---
	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	float AggroRadius = 800.0f;

	// --- Stats ---
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	FMutationStats BaseStats;
};