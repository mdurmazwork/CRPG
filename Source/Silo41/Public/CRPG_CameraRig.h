#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRPG_CameraRig.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * ACRPG_CameraRig (Revize v6: Inventory Mode Added)
 */
UCLASS()
class SILO41_API ACRPG_CameraRig : public AActor
{
	GENERATED_BODY()

public:
	ACRPG_CameraRig();

	virtual void Tick(float DeltaTime) override;

	void AddYawInput(float Val);
	void AddZoomInput(float Val);
	void AddPanInput(const FVector2D& PanInput);
	void ToggleCameraLock();
	bool IsCameraLocked() const { return bIsCameraLocked; }

	// --- CAMERA MODES ---
	void EnableDialogueMode(AActor* NpcActor);
	void DisableDialogueMode();

	// [YENÝ] Envanter Modu (Karakteri sola yaslar)
	void EnableInventoryMode();
	void DisableInventoryMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* MainCamera;

	UPROPERTY()
	AActor* TargetActor;

	// --- SETTINGS ---
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraSmoothSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float RotationSmoothSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ZoomSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinZoom = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxZoom = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float PanSpeed = 1500.0f;

	// --- DIALOGUE MODE ---
	UPROPERTY(EditAnywhere, Category = "Camera Settings|Dialogue")
	float DialogueZoomDistance = 750.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings|Dialogue")
	float DialogueScreenOffset = 250.0f; // Saða kaydýrýr

	UPROPERTY(EditAnywhere, Category = "Camera Settings|Dialogue")
	float DialogueTransitionSpeed = 2.0f;

	// --- [YENÝ] INVENTORY MODE ---

	// Envanter açýlýnca ne kadar yaklaþsýn?
	UPROPERTY(EditAnywhere, Category = "Camera Settings|Inventory")
	float InventoryZoomDistance = 600.0f;

	// Envanter açýlýnca kamera ne kadar SAÐA kaysýn? (Karakter SOLA geçer)
	// Görseline göre karakter solda olacaðý için kamerayý saða itmeliyiz.
	UPROPERTY(EditAnywhere, Category = "Camera Settings|Inventory")
	float InventoryScreenOffset = 300.0f;

private:
	float TargetYaw;
	float TargetArmLength;
	float SavedArmLength;

	FVector TargetLocation;
	bool bIsCameraLocked;

	// Modes
	bool bIsDialogueActive;
	bool bIsInventoryActive; // [YENÝ]

	UPROPERTY()
	AActor* DialogueNPC;
};