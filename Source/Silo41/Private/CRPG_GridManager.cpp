#include "CRPG_GridManager.h"
#include "CRPG_GameMode.h"
#include "CRPG_CharacterBase.h"
#include "CRPG_TurnManager.h" // Eklendi 
#include "Kismet/GameplayStatics.h"

ACRPG_GridManager::ACRPG_GridManager()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	CombatBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatBounds"));
	CombatBounds->SetupAttachment(RootComponent);
	CombatBounds->SetBoxExtent(FVector(1000.0f, 1000.0f, 100.0f));
	CombatBounds->SetCollisionProfileName(TEXT("NoCollision"));

	GridVisuals = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GridVisuals"));
	GridVisuals->SetupAttachment(RootComponent);
	GridVisuals->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bCombatStarted = false;
}

void ACRPG_GridManager::BeginPlay()
{
	Super::BeginPlay();

	GridVisuals->SetVisibility(false);
	GridVisuals->ClearInstances();
}

void ACRPG_GridManager::TriggerCombatFromEnemy(AActor* EnemyInstigator)
{
	if (bCombatStarted || !EnemyInstigator) return;

	UE_LOG(LogTemp, Warning, TEXT("SILO-41 COMBAT: Triggered by %s. Initializing Arena..."), *EnemyInstigator->GetName());

	FVector EnemyLoc = EnemyInstigator->GetActorLocation();

	// --- ARENAYI TOPLA (KATILIMCILARI BUL) ---
	// GridManager'ý konumlandýrmadan önce kimlerin savaþacaðýný bulalým.
	// Þimdilik basitçe: Triggerlayan Enemy ve Player.
	// Ýleride "Overlap" ile alandaki herkesi alacaðýz.

	TArray<ACRPG_CharacterBase*> Participants;

	// 1. Düþmaný Ekle
	if (ACRPG_CharacterBase* EnemyChar = Cast<ACRPG_CharacterBase>(EnemyInstigator))
	{
		Participants.Add(EnemyChar);
	}

	// 2. Oyuncuyu Ekle
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (ACRPG_CharacterBase* CRPGPlayer = Cast<ACRPG_CharacterBase>(PlayerChar))
	{
		Participants.Add(CRPGPlayer);
	}

	// --- GRID KONUMLANDIRMA ---
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	for (AActor* Char : Participants) QueryParams.AddIgnoredActor(Char);

	FHitResult Hit;
	FVector TraceStart = EnemyLoc;
	FVector TraceEnd = EnemyLoc - FVector(0, 0, 1000.0f);

	FVector NewGridCenter = EnemyLoc;

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
	{
		NewGridCenter = Hit.Location;
	}
	else
	{
		NewGridCenter.Z -= 90.0f;
	}

	SetActorLocation(NewGridCenter);

	// --- GAME MODE & TURN MANAGER BAÞLATMA ---
	ACRPG_GameMode* GM = Cast<ACRPG_GameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->SetGameState(ECRPG_GameState::Combat);
	}

	GenerateGrid();
	SnapAllCharactersToGrid();

	// TURN MANAGER'I DOÐUR VE BAÞLAT
	FVector TM_SpawnLoc = GetActorLocation(); // GridManager ile ayný yerde doðsun, önemsiz.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ACRPG_TurnManager* TurnManager = GetWorld()->SpawnActor<ACRPG_TurnManager>(ACRPG_TurnManager::StaticClass(), TM_SpawnLoc, FRotator::ZeroRotator, SpawnParams);

	if (TurnManager)
	{
		TurnManager->StartCombat(Participants);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SILO-41 ERROR: Failed to spawn TurnManager!"));
	}

	bCombatStarted = true;
}

void ACRPG_GridManager::GenerateGrid()
{
	GridVisuals->ClearInstances();
	GridVisuals->SetVisibility(true);

	FVector BoxOrigin = CombatBounds->GetComponentLocation();
	FVector BoxExtent = CombatBounds->GetScaledBoxExtent();

	GridOrigin = BoxOrigin - FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(this, ACRPG_CharacterBase::StaticClass(), AllCharacters);
	for (AActor* Char : AllCharacters)
	{
		QueryParams.AddIgnoredActor(Char);
	}

	for (int32 x = 0; x < GridSizeX; x++)
	{
		for (int32 y = 0; y < GridSizeY; y++)
		{
			float PosX = (x * TileSize) + (TileSize / 2.0f);
			float PosY = (y * TileSize) + (TileSize / 2.0f);

			FVector TileLocation = GridOrigin + FVector(PosX, PosY, 0.0f);

			FHitResult Hit;
			FVector TraceStart = TileLocation + FVector(0, 0, 500.0f);
			FVector TraceEnd = TileLocation - FVector(0, 0, 500.0f);

			if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
			{
				TileLocation.Z = Hit.Location.Z + 2.0f;
			}

			FTransform TileTransform;
			TileTransform.SetLocation(TileLocation);
			TileTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			GridVisuals->AddInstanceWorldSpace(TileTransform);
		}
	}
}

FVector ACRPG_GridManager::GetNearestTileLocation(FVector WorldLocation) const
{
	FVector LocalPos = WorldLocation - GridOrigin;

	int32 IndexX = FMath::RoundToInt((LocalPos.X - (TileSize / 2.0f)) / TileSize);
	int32 IndexY = FMath::RoundToInt((LocalPos.Y - (TileSize / 2.0f)) / TileSize);

	IndexX = FMath::Clamp(IndexX, 0, GridSizeX - 1);
	IndexY = FMath::Clamp(IndexY, 0, GridSizeY - 1);

	float WorldX = (IndexX * TileSize) + (TileSize / 2.0f);
	float WorldY = (IndexY * TileSize) + (TileSize / 2.0f);

	return GridOrigin + FVector(WorldX, WorldY, LocalPos.Z);
}

bool ACRPG_GridManager::IsLocationInsideGrid(FVector WorldLocation) const
{
	if (!bCombatStarted) return false;

	FVector LocalPos = WorldLocation - GridOrigin;

	float IndexX = (LocalPos.X) / TileSize;
	float IndexY = (LocalPos.Y) / TileSize;

	if (IndexX < 0 || IndexX >= GridSizeX || IndexY < 0 || IndexY >= GridSizeY)
	{
		return false;
	}

	return true;
}

void ACRPG_GridManager::SnapAllCharactersToGrid()
{
	FVector BoxOrigin = CombatBounds->GetComponentLocation();
	FVector BoxExtent = CombatBounds->GetScaledBoxExtent();

	TArray<FHitResult> HitResults;
	FCollisionShape BoxShape;
	BoxShape.SetBox(FVector3f(BoxExtent));

	GetWorld()->SweepMultiByChannel(
		HitResults,
		BoxOrigin,
		BoxOrigin,
		FQuat::Identity,
		ECC_Pawn,
		BoxShape
	);

	for (const FHitResult& Hit : HitResults)
	{
		ACRPG_CharacterBase* Character = Cast<ACRPG_CharacterBase>(Hit.GetActor());
		if (Character)
		{
			FVector CurrentLoc = Character->GetActorLocation();
			FVector SnapLoc = GetNearestTileLocation(CurrentLoc);
			SnapLoc.Z = CurrentLoc.Z;

			Character->SetActorLocation(SnapLoc);
		}
	}
}