#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CRPG_MutationData.h" 
#include "CRPG_AttributeComponent.h" 
#include "CRPG_InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "CRPG_DialogueData.h" 
#include "CRPG_CharacterBase.generated.h"

// Takým Tanýmý 
UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Player  UMETA(DisplayName = "Player Team"),
	Enemy   UMETA(DisplayName = "Enemy Team"),
	Neutral UMETA(DisplayName = "Neutral / NPC")
};

UCLASS()
class SILO41_API ACRPG_CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACRPG_CharacterBase();

	virtual void Tick(float DeltaTime) override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCRPG_AttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCRPG_InventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* AggroSphere;

	// --- Data Injection ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41 Configuration")
	UCRPG_MutationData* MutationData;

	// [YENÝ] Karakterin konuþacaðý varsayýlan diyalog verisi
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41 Configuration")
	UCRPG_DialogueData* DefaultDialogue;

	// [YENÝ] Son konuþulan diyaloðun ID'si (Hafýza)
	UPROPERTY(VisibleAnywhere, Category = "Silo41|State")
	int32 LastDialogueNodeID; // Varsayýlan: -1

	// Takým Seçimi
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41 Configuration")
	ETeamType TeamID;

	bool IsPlayerCharacter() const;

	void SetHighlight(bool bIsActive);

	// Etkileþim Fonksiyonu (Diyalog baþlatmak için)
	void InteractWithCharacter();

protected:
	virtual void BeginPlay() override;
	void InitCharacter();

	UFUNCTION()
	void ActivateAggroSystem();

	UFUNCTION()
	void OnAggroOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void StartCombatEncounter();

	// Grid Snap Mantýðý
	void UpdateGridSnap(float DeltaTime);

private:
	bool bIsSnappingToTile;
};