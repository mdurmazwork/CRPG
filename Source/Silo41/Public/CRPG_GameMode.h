#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CRPG_GameMode.generated.h"

UENUM(BlueprintType)
enum class ECRPG_GameState : uint8
{
	Exploration UMETA(DisplayName = "Exploration Mode"),
	Combat      UMETA(DisplayName = "Combat Mode")
};

UCLASS()
class SILO41_API ACRPG_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACRPG_GameMode();

	// --- State Management --- 
	UFUNCTION(BlueprintCallable, Category = "Silo41|GameMode")
	ECRPG_GameState GetCurrentGameState() const { return CurrentGameState; }

	UFUNCTION(BlueprintCallable, Category = "Silo41|GameMode")
	void SetGameState(ECRPG_GameState NewState);

protected:
	virtual void BeginPlay() override;

private:
	ECRPG_GameState CurrentGameState;

	// State deðiþtiðinde yapýlacak iþlemler
	void HandleExplorationStart();
	void HandleCombatStart();
};