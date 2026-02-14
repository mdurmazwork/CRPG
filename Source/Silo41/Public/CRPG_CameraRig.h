#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRPG_CameraRig.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * ACRPG_CameraRig (Revize v2: Free Roam & Lock System)
 * * ÝZOMETRÝK KAMERA SÝSTEMÝ
 * Görevi: Oyuncudan baðýmsýz hareket edebilen, istendiðinde karaktere kilitlenen kamera.
 * Kontrol:
 * - WASD: Kamerayý kaydýr (Pan).
 * - Q/E: Kamerayý döndür (Rotate).
 * - Scroll: Yakýnlaþ/Uzaklaþ (Zoom).
 * - Space: Karaktere kilitlen/Serbest býrak (Lock/Unlock).
 */
UCLASS()
class SILO41_API ACRPG_CameraRig : public AActor
{
	GENERATED_BODY()

public:
	ACRPG_CameraRig();

	virtual void Tick(float DeltaTime) override;

	// PlayerController tarafýndan çaðrýlýr.
	void AddYawInput(float Val);
	void AddZoomInput(float Val);

	// YENÝ: Kamerayý WASD ile kaydýrma
	void AddPanInput(const FVector2D& PanInput);

	// YENÝ: Kamerayý karaktere kilitle veya serbest býrak
	void ToggleCameraLock();

	// YENÝ: Kameranýn þu an kilitli olup olmadýðýný döner
	bool IsCameraLocked() const { return bIsCameraLocked; }

protected:
	virtual void BeginPlay() override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* MainCamera;

	// --- Settings ---

	// Kameranýn takip edeceði aktör (Genellikle CharacterBase)
	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float CameraSmoothSpeed = 10.0f; // Pan yaparken daha hýzlý tepki vermesi için artýrdýk

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float RotationSmoothSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float ZoomSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinZoom = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxZoom = 2500.0f;

	// YENÝ: WASD ile kaydýrma hýzý
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float PanSpeed = 1500.0f;

private:
	// Hedeflenen rotasyon (Yumuþak geçiþ için)
	float TargetYaw;

	// Hedeflenen Zoom mesafesi
	float TargetArmLength;

	// YENÝ: Hedeflenen Dünya Pozisyonu (Kameranýn gitmek istediði yer)
	FVector TargetLocation;

	// YENÝ: Kamera karaktere kilitli mi?
	bool bIsCameraLocked;
};