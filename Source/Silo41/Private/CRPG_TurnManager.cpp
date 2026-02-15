#include "CRPG_TurnManager.h"
#include "CRPG_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ACRPG_TurnManager::ACRPG_TurnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentTurnIndex = -1;
	RoundCount = 0;
}

void ACRPG_TurnManager::BeginPlay()
{
	Super::BeginPlay();
}

void ACRPG_TurnManager::StartCombat(const TArray<ACRPG_CharacterBase*>& Combatants)
{
	if (Combatants.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SILO-41 ERROR: TurnManager started with NO combatants!"));
		return;
	}

	InitiativeOrder = Combatants;
	CurrentTurnIndex = -1;
	RoundCount = 1;

	// Basit Sýralama: Oyuncuyu her zaman listenin baþýna al (Debug kolaylýðý için) 
	// Ýleride burayý "Initiative Stat"ýna göre BubbleSort yapacaðýz.
	InitiativeOrder.Sort([](const ACRPG_CharacterBase& A, const ACRPG_CharacterBase& B) {
		return A.IsPlayerCharacter(); // Player true ise baþa gelir.
		});

	UE_LOG(LogTemp, Warning, TEXT("SILO-41 COMBAT: Combat Started! Round 1. Participants: %d"), InitiativeOrder.Num());

	// Ýlk sýrayý ver
	NextTurn();
}

void ACRPG_TurnManager::NextTurn()
{
	CurrentTurnIndex++;

	// Liste sonuna geldik mi?
	if (CurrentTurnIndex >= InitiativeOrder.Num())
	{
		CurrentTurnIndex = 0;
		RoundCount++;
		UE_LOG(LogTemp, Warning, TEXT("SILO-41 COMBAT: --- NEW ROUND: %d ---"), RoundCount);
	}

	ACRPG_CharacterBase* ActiveChar = InitiativeOrder[CurrentTurnIndex];

	// Eðer karakter ölmüþse veya yok olmuþsa pas geç
	if (!ActiveChar || ActiveChar->IsPendingKillPending())
	{
		NextTurn();
		return;
	}

	StartTurn(ActiveChar);
}

void ACRPG_TurnManager::StartTurn(ACRPG_CharacterBase* ActiveCharacter)
{
	UE_LOG(LogTemp, Log, TEXT("SILO-41 TURN: It is now %s's turn."), *ActiveCharacter->GetName());

	// 1. OYUNCU MU?
	if (ActiveCharacter->IsPlayerCharacter())
	{
		ACRPG_PlayerController* PC = Cast<ACRPG_PlayerController>(ActiveCharacter->GetController());
		if (PC)
		{
			// Oyuncunun kontrolünü aç
			PC->SetIsTurnActive(true);
			UE_LOG(LogTemp, Log, TEXT("--> WAITING FOR PLAYER INPUT..."));
		}
	}
	// 2. DÜÞMAN MI?
	else
	{
		// Oyuncunun kontrolünü kapat (Sýra düþmanda)
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (ACRPG_PlayerController* SiloPC = Cast<ACRPG_PlayerController>(PlayerController))
		{
			SiloPC->SetIsTurnActive(false);
		}

		// Yapay Zeka Mantýðýný Çalýþtýr (Þimdilik Simülasyon)
		ProcessAITurn(ActiveCharacter);
	}
}

void ACRPG_TurnManager::ProcessAITurn(ACRPG_CharacterBase* AICharacter)
{
	UE_LOG(LogTemp, Log, TEXT("--> AI %s is thinking..."), *AICharacter->GetName());

	// Simülasyon: Düþman 2 saniye düþünür ve sýrayý salar.
	// Ýleride burada "AIController->Attack()" çaðýracaðýz.
	FTimerHandle TimerHandle_AITurn;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_AITurn, this, &ACRPG_TurnManager::NextTurn, 2.0f, false);
}