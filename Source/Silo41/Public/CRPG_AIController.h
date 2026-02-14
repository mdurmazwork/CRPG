#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "CRPG_AIController.generated.h"

// Ýleri tanýmlamalar (Forward Declaration)
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class ACRPG_CharacterBase;

/**
 * UCRPG_AIController
 * * UTILITY AI BRAIN
 * Görevi: Behavior Tree kullanmadan, anlýk duruma göre (Can, Mesafe, AP) puanlama yaparak
 * en mantýklý kararý veren C++ yapay zeka kontrolcüsü.
 */
UCLASS()
class SILO41_API ACRPG_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ACRPG_AIController();

	virtual void Tick(float DeltaTime) override;

	// --- SENSÖRLER (EYES) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	// --- STATE ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
	AActor* CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
	bool bIsCombatActive;

	// --- DECISION MAKING (BRAIN) ---

	// En mantýklý hareketi seç ve uygula
	UFUNCTION(BlueprintCallable, Category = "Silo41|AI")
	void DecideBestAction();

protected:
	virtual void BeginPlay() override;

	// Birisi görüþ alanýna girdiðinde/çýktýðýnda tetiklenir
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;

private:
	// Yardýmcý fonksiyon: Hedef düþman mý?
	bool IsHostile(AActor* TargetActor) const;
};