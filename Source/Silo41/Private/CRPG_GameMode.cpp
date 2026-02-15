#include "CRPG_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ACRPG_GameMode::ACRPG_GameMode()
{
	CurrentGameState = ECRPG_GameState::Exploration;
}

void ACRPG_GameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("SILO-41 SYSTEM: Game Mode Started. Initial State: Exploration"));
}

void ACRPG_GameMode::SetGameState(ECRPG_GameState NewState)
{
	if (CurrentGameState == NewState) return;

	CurrentGameState = NewState;

	switch (CurrentGameState)
	{
	case ECRPG_GameState::Exploration:
		HandleExplorationStart();
		break;

	case ECRPG_GameState::Combat:
		HandleCombatStart();
		break;
	}
}

void ACRPG_GameMode::HandleExplorationStart()
{
	UE_LOG(LogTemp, Log, TEXT("SILO-41 SYSTEM: Switched to EXPLORATION Mode."));
	// UI ve Kontrolleri aç
}

void ACRPG_GameMode::HandleCombatStart()
{
	UE_LOG(LogTemp, Log, TEXT("SILO-41 SYSTEM: Switched to COMBAT Mode."));

	// 1. Oyuncunun hareketini durdur (Mouse ile týklanan yere gitmeyi iptal et) 
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->StopMovement();
		// Ýleride burada Input Mapping Context deðiþtirip "Savaþ Kontrolleri"ne geçeceðiz.
	}
}