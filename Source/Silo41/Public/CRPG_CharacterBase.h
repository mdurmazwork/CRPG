#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CRPG_MutationData.h" 
#include "CRPG_AttributeComponent.h" 
#include "CRPG_InventoryComponent.h" // YENÝ: Envanter Eriþimi
#include "Components/SphereComponent.h"
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

	// YENÝ: Envanter Bileþeni
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCRPG_InventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* AggroSphere;

	// --- Data Injection ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41 Configuration")
	UCRPG_MutationData* MutationData;

	// Takým Seçimi (Editörden ayarlanýr)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41 Configuration")
	ETeamType TeamID;

	bool IsPlayerCharacter() const;

	// Highlight Fonksiyonu (InteractableActor'dan uyarlandý)
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