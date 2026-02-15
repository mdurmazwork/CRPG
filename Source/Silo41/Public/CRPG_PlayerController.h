#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CRPG_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ACRPG_CameraRig;

UCLASS()
class SILO41_API ACRPG_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACRPG_PlayerController();

	virtual void Tick(float DeltaTime) override;

	// --- INPUT ACTIONS --- 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveClickAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* HighlightAction;

	// [YENÝ] Envanter Tuþu (I)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InventoryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CameraPanAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CameraLockAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* EndTurnAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CameraRotateAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CameraZoomAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* RotateMouseHoldAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* RotateMouseAxisAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float KeyboardRotationSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float MouseRotationSpeed = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Silo41|Interaction")
	float InteractionStoppingDistance = 180.0f;

	void SetIsTurnActive(bool bActive);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnClickTriggered(const FInputActionValue& Value);
	void OnHighlightPressed(const FInputActionValue& Value);
	void OnHighlightReleased(const FInputActionValue& Value);

	// [YENÝ]
	void OnInventoryKeyPressed(const FInputActionValue& Value);

	void OnCameraPan(const FInputActionValue& Value);
	void OnCameraLock(const FInputActionValue& Value);
	void OnEndTurnTriggered(const FInputActionValue& Value);
	void OnCameraRotateKeyboard(const FInputActionValue& Value);
	void OnCameraZoom(const FInputActionValue& Value);
	void OnRotateMouseHold(const FInputActionValue& Value);
	void OnRotateMouseRelease(const FInputActionValue& Value);
	void OnRotateMouseAxis(const FInputActionValue& Value);

	// --- Interaction Logic ---
	void MoveToInteractable(AActor* Target);
	void CheckInteractionDistance();
	void PerformInteraction();
	void StopInteractionApproach();

	ACRPG_CameraRig* ControlledCameraRig;
	bool bIsMiddleMousePressed;
	bool bIsTurnActive;

	UPROPERTY()
	AActor* CachedTargetActor;
	bool bIsApproachingTarget;
};