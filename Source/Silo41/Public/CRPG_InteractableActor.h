#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRPG_ItemData.h" // Item Tanýmasý için
#include "CRPG_InteractableActor.generated.h"

UCLASS()
class SILO41_API ACRPG_InteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ACRPG_InteractableActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Interaction")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Interaction")
	FText ActionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Interaction")
	bool bIsInteractable;

	// --- YENÝ: LOOT SÝSTEMÝ (Basit Test Ýçin) --- 

	// Bu objeye týklandýðýnda oyuncuya verilecek eþya (Boþ býrakýlýrsa vermez)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Loot")
	UCRPG_ItemData* ItemToGive;

	// Kaç adet verilecek?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Loot")
	int32 ItemQuantity = 1;

	// Eþya verildikten sonra obje yok olsun mu? (Potion ise evet, Sandýk ise hayýr)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Silo41|Loot")
	bool bDestroyOnInteract = false;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Silo41|Interaction")
	void OnInteract(APawn* InstigatorPawn);

	void SetHighlight(bool bIsActive);

protected:
	virtual void BeginPlay() override;
};