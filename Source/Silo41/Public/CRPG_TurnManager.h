#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRPG_CharacterBase.h"
#include "CRPG_TurnManager.generated.h"

UCLASS()
class SILO41_API ACRPG_TurnManager : public AActor
{
	GENERATED_BODY()

public:
	ACRPG_TurnManager();

	// Savaþi baþlatýr ve katýlýmcý listesini alýr.
	void StartCombat(const TArray<ACRPG_CharacterBase*>& Combatants);

	// Sýradaki karaktere geçer.
	UFUNCTION(BlueprintCallable, Category = "Silo41|Combat")
	void NextTurn();

protected:
	virtual void BeginPlay() override;

private:
	// Savaþanlarýn listesi (Sýralý)
	UPROPERTY()
	TArray<ACRPG_CharacterBase*> InitiativeOrder;

	// Þu an kimin sýrasý? (Index)
	int32 CurrentTurnIndex;

	// Tur sayýsý
	int32 RoundCount;

	// Sýrayý baþlatýr
	void StartTurn(ACRPG_CharacterBase* ActiveCharacter);

	// Yapay zeka hamlesini simüle eder (Þimdilik)
	void ProcessAITurn(ACRPG_CharacterBase* AICharacter);
};