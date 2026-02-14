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

	virtual void Tick(float DeltaTime) override; // YENÝ: Mesafe kontrolü için Tick açýldý

	// --- INPUT ACTIONS ---
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveClickAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* HighlightAction;

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

	// Etkileþim için ne kadar yaklaþmalý? (cm)
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

	void OnCameraPan(const FInputActionValue& Value);
	void OnCameraLock(const FInputActionValue& Value);
	void OnEndTurnTriggered(const FInputActionValue& Value);
	void OnCameraRotateKeyboard(const FInputActionValue& Value);
	void OnCameraZoom(const FInputActionValue& Value);
	void OnRotateMouseHold(const FInputActionValue& Value);
	void OnRotateMouseRelease(const FInputActionValue& Value);
	void OnRotateMouseAxis(const FInputActionValue& Value);

	// --- YENÝ: Yaklaþ ve Etkileþ Sistemi ---
	void MoveToInteractable(AActor* Target);
	void CheckInteractionDistance();
	void PerformInteraction();
	void StopInteractionApproach();

	ACRPG_CameraRig* ControlledCameraRig;
	bool bIsMiddleMousePressed;
	bool bIsTurnActive;

	// Hafýzadaki Hedef
	UPROPERTY()
	AActor* CachedTargetActor;

	// Þu an hedefe doðru yürüyor muyuz?
	bool bIsApproachingTarget;
};