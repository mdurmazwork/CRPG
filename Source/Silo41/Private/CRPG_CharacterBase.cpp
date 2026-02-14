#include "CRPG_CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "CRPG_GridManager.h"
#include "CRPG_GameMode.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h" 
#include "Components/PrimitiveComponent.h"

// UI BAÐLANTILARI
#include "CRPG_HUD.h"
#include "CRPG_DialogueWidget.h"
#include "GameFramework/PlayerController.h"

ACRPG_CharacterBase::ACRPG_CharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Attribute Component
	AttributeComp = CreateDefaultSubobject<UCRPG_AttributeComponent>(TEXT("AttributeComp"));

	// Inventory Component
	InventoryComp = CreateDefaultSubobject<UCRPG_InventoryComponent>(TEXT("InventoryComp"));

	// Aggro Sphere
	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(RootComponent);
	AggroSphere->SetCollisionProfileName(TEXT("Trigger"));
	AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AggroSphere->SetSphereRadius(1.0f);

	// Movement & Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	GetCharacterMovement()->MaxAcceleration = 8000.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 8000.0f;
	GetCharacterMovement()->GroundFriction = 20.0f;

	bIsSnappingToTile = false;
	TeamID = ETeamType::Enemy;

	// Hafýza Baþlatma
	LastDialogueNodeID = -1;
}

void ACRPG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AggroSphere)
	{
		AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AggroSphere->SetSphereRadius(1.0f);
		AggroSphere->SetHiddenInGame(true);
	}

	InitCharacter();
	SetHighlight(false);
}

void ACRPG_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateGridSnap(DeltaTime);
}

void ACRPG_CharacterBase::UpdateGridSnap(float DeltaTime)
{
	ACRPG_GameMode* GM = Cast<ACRPG_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM || GM->GetCurrentGameState() != ECRPG_GameState::Combat || GetCharacterMovement()->IsFalling())
	{
		bIsSnappingToTile = false;
		return;
	}

	bool bNavMeshMoving = false;
	bool bInputMoving = false;

	AController* MyController = GetController();
	if (MyController)
	{
		UPathFollowingComponent* PathComp = MyController->FindComponentByClass<UPathFollowingComponent>();
		if (PathComp && PathComp->GetStatus() == EPathFollowingStatus::Moving)
		{
			bNavMeshMoving = true;
		}
	}

	if (GetCharacterMovement()->GetCurrentAcceleration().SizeSquared() > 10.0f)
	{
		bInputMoving = true;
	}

	if (bNavMeshMoving || bInputMoving)
	{
		bIsSnappingToTile = false;
		return;
	}

	AActor* GridActor = UGameplayStatics::GetActorOfClass(this, ACRPG_GridManager::StaticClass());
	ACRPG_GridManager* GridManager = Cast<ACRPG_GridManager>(GridActor);
	if (!GridManager) return;

	FVector MyLoc = GetActorLocation();
	FVector TargetTileLoc = GridManager->GetNearestTileLocation(MyLoc);
	TargetTileLoc.Z = MyLoc.Z;

	float Distance = FVector::Dist2D(MyLoc, TargetTileLoc);

	if (Distance > 0.1f)
	{
		if (Distance < 60.0f)
		{
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
			bIsSnappingToTile = true;

			FVector NewLoc = FMath::VInterpConstantTo(MyLoc, TargetTileLoc, DeltaTime, 800.0f);
			SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			bIsSnappingToTile = false;
		}
	}
	else
	{
		if (bIsSnappingToTile)
		{
			SetActorLocation(TargetTileLoc, false, nullptr, ETeleportType::TeleportPhysics);
			bIsSnappingToTile = false;
		}
	}
}

void ACRPG_CharacterBase::InitCharacter()
{
	if (!MutationData)
	{
		return;
	}

	if (MutationData->SkeletalMesh) GetMesh()->SetSkeletalMesh(MutationData->SkeletalMesh);
	if (MutationData->AnimClass) GetMesh()->SetAnimInstanceClass(MutationData->AnimClass);

	if (AttributeComp)
	{
		AttributeComp->InitializeStats(MutationData->BaseStats);
		GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetMovementSpeed();
	}

	if (IsPlayerCharacter())
	{
		TeamID = ETeamType::Player;
		if (AggroSphere)
		{
			AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AggroSphere->SetGenerateOverlapEvents(false);
		}
	}
	else
	{
		if (TeamID == ETeamType::Enemy)
		{
			FTimerHandle TimerHandle_Aggro;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Aggro, this, &ACRPG_CharacterBase::ActivateAggroSystem, 0.5f, false);
		}
	}
}

void ACRPG_CharacterBase::ActivateAggroSystem()
{
	if (!AggroSphere || !MutationData) return;

	AggroSphere->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	AggroSphere->SetSphereRadius(MutationData->AggroRadius);

	AggroSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ACRPG_CharacterBase::OnAggroOverlap);
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ACRPG_CharacterBase::OnAggroOverlap);

	AggroSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AggroSphere->UpdateOverlaps();
}

bool ACRPG_CharacterBase::IsPlayerCharacter() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return (PlayerPawn && PlayerPawn == this);
}

void ACRPG_CharacterBase::OnAggroOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	ACRPG_CharacterBase* Intruder = Cast<ACRPG_CharacterBase>(OtherActor);
	if (!Intruder) return;

	if (Intruder->IsPlayerCharacter() && TeamID == ETeamType::Enemy)
	{
		if (OtherComp != Intruder->GetCapsuleComponent()) return;

		UE_LOG(LogTemp, Warning, TEXT("SILO-41 COMBAT: Enemy saw Player! Starting Combat."));
		StartCombatEncounter();
	}
}

void ACRPG_CharacterBase::StartCombatEncounter()
{
	ACRPG_GameMode* GM = Cast<ACRPG_GameMode>(UGameplayStatics::GetGameMode(this));

	if (GM && GM->GetCurrentGameState() == ECRPG_GameState::Combat) return;

	AActor* GridActor = UGameplayStatics::GetActorOfClass(this, ACRPG_GridManager::StaticClass());
	if (ACRPG_GridManager* GridManager = Cast<ACRPG_GridManager>(GridActor))
	{
		GridManager->TriggerCombatFromEnemy(this);
	}
}

void ACRPG_CharacterBase::SetHighlight(bool bIsActive)
{
	TArray<UPrimitiveComponent*> AllComponents;
	GetComponents<UPrimitiveComponent>(AllComponents);

	for (UPrimitiveComponent* Comp : AllComponents)
	{
		if (Comp)
		{
			Comp->SetRenderCustomDepth(bIsActive);
			if (bIsActive)
			{
				Comp->SetCustomDepthStencilValue(1);
			}
		}
	}
}

// [GÜNCELLENDÝ] Kaldýðý yerden devam etme (LastDialogueNodeID) eklendi
void ACRPG_CharacterBase::InteractWithCharacter()
{
	if (TeamID == ETeamType::Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("COMBAT: You clicked on an Enemy! Attacking..."));
		StartCombatEncounter();
		return;
	}

	if (DefaultDialogue)
	{
		APlayerController* PlayerPC = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerPC)
		{
			ACRPG_HUD* HUD = Cast<ACRPG_HUD>(PlayerPC->GetHUD());
			if (HUD)
			{
				if (UUserWidget* RawWidget = HUD->GetDialogueWidget())
				{
					if (UCRPG_DialogueWidget* DlgWidget = Cast<UCRPG_DialogueWidget>(RawWidget))
					{
						HUD->ShowDialogue(true);

						// Hafýzadaki ID ile baþlat. Eðer -1 ise en baþtan baþlar.
						DlgWidget->StartDialogue(DefaultDialogue, LastDialogueNodeID);

						UE_LOG(LogTemp, Log, TEXT("INTERACTION: Resuming dialogue at Node %d"), LastDialogueNodeID);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("INTERACTION ERROR: HUD has no Dialogue Widget created!"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC INTERACTION: Hello traveler! I am %s. (No Dialogue Data Assigned)"), *GetName());
	}
}