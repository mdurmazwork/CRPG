#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CRPG_AbilityData.h" // Yetenek verisi
#include "CRPG_ActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionPointsChanged, int32, NewAP, int32, MaxAP);

/**
 * UCRPG_ActionComponent
 * * SAVAÞ MOTORU
 * Görevi:
 * 1. Karakterin sahip olduðu yetenekleri (Loadout) tutar.
 * 2. Action Point (AP) yönetimini yapar.
 * 3. Yetenekleri çalýþtýrýr (Animasyon, VFX, Hasar).
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SILO41_API UCRPG_ActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCRPG_ActionComponent();

	// --- CONFIG ---

	// Karakterin baþlangýçta sahip olduðu yetenekler (Editörden atanýr)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Abilities")
	TArray<UCRPG_AbilityData*> DefaultAbilities;

	// Maksimum Action Point
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Stats")
	int32 MaxActionPoints = 4;

	// --- STATE ---

	// Þu anki Action Point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Silo41|State")
	int32 CurrentActionPoints;

	// --- EVENTS ---

	// UI güncellemek için Event (AP deðiþince tetiklenir)
	UPROPERTY(BlueprintAssignable, Category = "Silo41|Events")
	FOnActionPointsChanged OnActionPointsChanged;

	// --- ACTIONS ---

	// Tur baþýna AP'yi yeniler. (TurnManager çaðýracak)
	void ResetActionPoints();

	// Bir yeteneði çalýþtýrmayý dener.
	UFUNCTION(BlueprintCallable, Category = "Silo41|Combat")
	bool ExecuteAbility(UCRPG_AbilityData* AbilityToUse, AActor* TargetActor);

protected:
	virtual void BeginPlay() override;

	// DataAsset'teki yeteneði sahneye uygular
	void ApplyAbilityEffects(UCRPG_AbilityData* Ability, AActor* Target);
};