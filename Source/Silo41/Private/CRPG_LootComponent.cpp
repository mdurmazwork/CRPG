#include "CRPG_LootComponent.h"
#include "Math/UnrealMathUtility.h"

UCRPG_LootComponent::UCRPG_LootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bLooted = false;
	bOneTimeLoot = true;
}

void UCRPG_LootComponent::BeginPlay()
{
	Super::BeginPlay();
}

TArray<FLootResult> UCRPG_LootComponent::RollLoot()
{
	TArray<FLootResult> Results;

	if (bOneTimeLoot && bLooted)
	{
		UE_LOG(LogTemp, Warning, TEXT("LOOT: Already looted!"));
		return Results; // Boþ dön
	}

	UE_LOG(LogTemp, Log, TEXT("LOOT: Rolling dice for %d rules..."), LootTable.Num());

	for (const FLootRule& Rule : LootTable)
	{
		if (!Rule.ItemData) continue;

		// 1. Zar At (0.0 ile 1.0 arasý rastgele sayý) 
		float Roll = FMath::FRand();

		// 2. Þans Kontrolü
		if (Roll <= Rule.DropChance)
		{
			// 3. Miktar Belirle
			int32 Qty = FMath::RandRange(Rule.MinQuantity, Rule.MaxQuantity);

			FLootResult Result;
			Result.Item = Rule.ItemData;
			Result.Count = Qty;
			Results.Add(Result);

			UE_LOG(LogTemp, Log, TEXT("   -> SUCCESS: Dropped %d x %s (Chance: %.2f, Roll: %.2f)"), Qty, *Rule.ItemData->Name.ToString(), Rule.DropChance, Roll);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("   -> FAIL: %s (Chance: %.2f, Roll: %.2f)"), *Rule.ItemData->Name.ToString(), Rule.DropChance, Roll);
		}
	}

	if (bOneTimeLoot)
	{
		bLooted = true;
	}

	return Results;
}