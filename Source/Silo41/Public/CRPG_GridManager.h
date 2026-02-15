#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "CRPG_GridManager.generated.h"

UCLASS()
class SILO41_API ACRPG_GridManager : public AActor
{
	GENERATED_BODY()

public:
	ACRPG_GridManager();

	// --- Config ---
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	int32 GridSizeX = 10;

	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	int32 GridSizeY = 10;

	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	float TileSize = 200.0f;

	// --- Components --- 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CombatBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* GridVisuals;

protected:
	virtual void BeginPlay() override;

public:
	void TriggerCombatFromEnemy(AActor* EnemyInstigator);
	void GenerateGrid();

	// Konumun en yakýn tile merkezini verir (Snap için)
	FVector GetNearestTileLocation(FVector WorldLocation) const;

	// YENÝ: Verilen konumun Grid sýnýrlarý içinde olup olmadýðýný kontrol eder.
	bool IsLocationInsideGrid(FVector WorldLocation) const;

	void SnapAllCharactersToGrid();

private:
	bool bCombatStarted;
	FVector GridOrigin;
};