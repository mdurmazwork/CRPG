#include "CRPG_WorldPartitionManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

ACRPG_WorldPartitionManager::ACRPG_WorldPartitionManager()
{
	PrimaryActorTick.bCanEverTick = false; // Tick yerine Timer kullanýyoruz (Performans)

	SectorSize = 5000.0f; // 50 metrelik kareler
	CurrentSector = FIntPoint(0, 0);
	CheckInterval = 1.0f;
}

void ACRPG_WorldPartitionManager::BeginPlay()
{
	Super::BeginPlay();

	// Baþlangýçta oyuncunun nerede olduðunu bul
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerChar)
	{
		CurrentSector = GetSectorIndex(PlayerChar->GetActorLocation());

		// Ýlk giriþte event tetikle (Baþlangýç bölgesi etkileri için)
		OnSectorChanged.Broadcast(CurrentSector, CurrentSector);

		UE_LOG(LogTemp, Log, TEXT("WORLD MANAGER: Initial Sector Detected: [%d, %d]"), CurrentSector.X, CurrentSector.Y);
	}

	// Periyodik kontrolü baþlat
	GetWorldTimerManager().SetTimer(TimerHandle_SectorCheck, this, &ACRPG_WorldPartitionManager::CheckPlayerSector, CheckInterval, true);
}

void ACRPG_WorldPartitionManager::CheckPlayerSector()
{
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerChar) return;

	FVector PlayerLoc = PlayerChar->GetActorLocation();
	FIntPoint NewSector = GetSectorIndex(PlayerLoc);

	// Eðer sektör deðiþmiþse
	if (NewSector != CurrentSector)
	{
		UE_LOG(LogTemp, Warning, TEXT("WORLD MANAGER: Sector Changed! OLD: [%d, %d] -> NEW: [%d, %d]"),
			CurrentSector.X, CurrentSector.Y, NewSector.X, NewSector.Y);

		// Event'i tetikle (Dinleyen sistemler: Quest, Survival, AI Spawner)
		OnSectorChanged.Broadcast(NewSector, CurrentSector);

		// Kaydý güncelle
		CurrentSector = NewSector;
	}
}

FIntPoint ACRPG_WorldPartitionManager::GetSectorIndex(FVector Location) const
{
	// Koordinatlarý sektör boyutuna böl ve tam sayýya yuvarla
	int32 X = FMath::FloorToInt(Location.X / SectorSize);
	int32 Y = FMath::FloorToInt(Location.Y / SectorSize);

	return FIntPoint(X, Y);
}