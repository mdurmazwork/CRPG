#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CRPG_InteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SILO41_API UCRPG_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCRPG_InteractionComponent();

	UPROPERTY(EditAnywhere, Category = "Silo41|Interaction")
	float HighlightRadius = 1500.0f;

	void ToggleHighlightMode(bool bEnable);
	void TryInteractAtLocation(FVector Location);

protected:
	virtual void BeginPlay() override;

private:
	// Türden baðýmsýz (Actor) tutuyoruz ki hem NPC hem Sandýk tutabilsin
	UPROPERTY()
	TArray<AActor*> HighlightedActors;
};