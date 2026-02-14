#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CRPG_ItemData.h"
#include "CRPG_LootComponent.generated.h"

USTRUCT(BlueprintType)
struct FLootRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCRPG_ItemData* ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MinQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MaxQuantity = 1;
};

// Basit veri taþýyýcý (Inventory baðýmlýlýðýný kýrmak için)
USTRUCT(BlueprintType)
struct FLootResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCRPG_ItemData* Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Count;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SILO41_API UCRPG_LootComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCRPG_LootComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Loot")
	TArray<FLootRule> LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Loot")
	bool bOneTimeLoot = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Silo41|State")
	bool bLooted;

	UFUNCTION(BlueprintCallable, Category = "Silo41|Loot")
	TArray<FLootResult> RollLoot();

protected:
	virtual void BeginPlay() override;
};