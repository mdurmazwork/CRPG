#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRPG_WorldPartitionManager.generated.h"

// Sektör deðiþtiðinde tetiklenecek olay (Survival ve Quest sistemleri bunu dinler)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSectorChanged, FIntPoint, NewSector, FIntPoint, OldSector);

UCLASS()
class SILO41_API ACRPG_WorldPartitionManager : public AActor
{
	GENERATED_BODY()

public:
	ACRPG_WorldPartitionManager();

	// --- Config ---

	// Bir mantýksal sektörün boyutu (cm cinsinden). Örn: 5000 = 50 metre.
	UPROPERTY(EditAnywhere, Category = "Silo41|World Settings")
	float SectorSize = 5000.0f;

	// Konum kontrolü ne sýklýkla yapýlsýn? (Saniye)
	UPROPERTY(EditAnywhere, Category = "Silo41|World Settings")
	float CheckInterval = 1.0f;

	// --- State ---

	// Þu an aktif olan sektör koordinatý (X, Y)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Silo41|State")
	FIntPoint CurrentSector;

	// --- Events ---

	UPROPERTY(BlueprintAssignable, Category = "Silo41|Events")
	FOnSectorChanged OnSectorChanged;

protected:
	virtual void BeginPlay() override;

private:
	// Oyuncunun konumunu kontrol eden zamanlayýcý
	FTimerHandle TimerHandle_SectorCheck;

	void CheckPlayerSector();

	// Konumu Sektör ID'sine çevirir (Örn: 1500, 1500 -> 0,0)
	FIntPoint GetSectorIndex(FVector Location) const;
};