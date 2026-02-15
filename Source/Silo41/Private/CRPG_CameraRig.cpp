#include "CRPG_CameraRig.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

ACRPG_CameraRig::ACRPG_CameraRig()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->SetAbsolute(false, true, false);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 1400.0f;
	CameraBoom->SetWorldRotation(FRotator(-45.0f, -45.0f, 0.0f));

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	MainCamera->bUsePawnControlRotation = false;

	TargetYaw = -45.0f;
	TargetArmLength = 1400.0f;
	SavedArmLength = 1400.0f;
	bIsCameraLocked = true;

	bIsDialogueActive = false;
	bIsInventoryActive = false;
	DialogueNPC = nullptr;
}

void ACRPG_CameraRig::BeginPlay()
{
	Super::BeginPlay();

	TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);

	if (TargetActor)
	{
		SetActorLocation(TargetActor->GetActorLocation());
		TargetLocation = GetActorLocation();
	}

	if (CameraBoom)
	{
		TargetYaw = CameraBoom->GetComponentRotation().Yaw;
		TargetArmLength = CameraBoom->TargetArmLength;
		SavedArmLength = TargetArmLength;
	}
}

void ACRPG_CameraRig::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// --- HEDEF POZÝSYON HESABI  ---

	// 1. Diyalog Modu
	if (bIsDialogueActive && TargetActor && DialogueNPC)
	{
		FVector P1 = TargetActor->GetActorLocation();
		FVector P2 = DialogueNPC->GetActorLocation();
		FVector MidPoint = (P1 + P2) * 0.5f;

		FRotator CamRot = CameraBoom->GetComponentRotation();
		FRotator YawOnlyRot(0.0f, CamRot.Yaw, 0.0f);
		FVector CameraRightVector = FRotationMatrix(YawOnlyRot).GetScaledAxis(EAxis::Y);

		TargetLocation = MidPoint + (CameraRightVector * DialogueScreenOffset);
	}
	// 2. Envanter Modu [YENÝ]
	else if (bIsInventoryActive && TargetActor)
	{
		// Sadece oyuncuya odaklan ama Offset uygula
		FVector PlayerLoc = TargetActor->GetActorLocation();

		FRotator CamRot = CameraBoom->GetComponentRotation();
		FRotator YawOnlyRot(0.0f, CamRot.Yaw, 0.0f);
		FVector CameraRightVector = FRotationMatrix(YawOnlyRot).GetScaledAxis(EAxis::Y);

		// Kamerayý saða kaydýr -> Karakter sola geçer
		TargetLocation = PlayerLoc + (CameraRightVector * InventoryScreenOffset);
	}
	// 3. Normal Kilitli Mod
	else if (bIsCameraLocked && TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}

	// Hareket Hýzý (Diyalog veya Envanterdeyse yavaþ geçiþ)
	float CurrentMoveSpeed = (bIsDialogueActive || bIsInventoryActive) ? DialogueTransitionSpeed : CameraSmoothSpeed;

	FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, CurrentMoveSpeed);
	SetActorLocation(NewLoc);

	// --- ROTASYON ---
	if (CameraBoom)
	{
		FRotator CurrentRot = CameraBoom->GetComponentRotation();
		FRotator TargetRot = FRotator(-45.0f, TargetYaw, 0.0f);
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSmoothSpeed);
		CameraBoom->SetWorldRotation(NewRot);
	}

	// --- ZOOM ---
	if (CameraBoom)
	{
		float ZoomTarget = TargetArmLength;

		if (bIsDialogueActive) ZoomTarget = DialogueZoomDistance;
		else if (bIsInventoryActive) ZoomTarget = InventoryZoomDistance;

		float InterpSpeed = (bIsDialogueActive || bIsInventoryActive) ? DialogueTransitionSpeed : CameraSmoothSpeed;

		float NewLen = FMath::FInterpTo(CameraBoom->TargetArmLength, ZoomTarget, DeltaTime, InterpSpeed);
		CameraBoom->TargetArmLength = NewLen;
	}
}

void ACRPG_CameraRig::EnableDialogueMode(AActor* NpcActor)
{
	if (!NpcActor) return;
	SavedArmLength = TargetArmLength;
	DialogueNPC = NpcActor;
	bIsDialogueActive = true;
	bIsInventoryActive = false; // Çakýþmayý önle
}

void ACRPG_CameraRig::DisableDialogueMode()
{
	bIsDialogueActive = false;
	DialogueNPC = nullptr;
	TargetArmLength = SavedArmLength;
	bIsCameraLocked = true;
}

// [YENÝ]
void ACRPG_CameraRig::EnableInventoryMode()
{
	SavedArmLength = TargetArmLength;
	bIsInventoryActive = true;
	bIsDialogueActive = false; // Çakýþmayý önle
}

// [YENÝ]
void ACRPG_CameraRig::DisableInventoryMode()
{
	bIsInventoryActive = false;
	TargetArmLength = SavedArmLength;
	bIsCameraLocked = true;
}

// ... Diðer fonksiyonlar ayný (AddPanInput, ToggleCameraLock vs.) ...
void ACRPG_CameraRig::AddPanInput(const FVector2D& PanInput)
{
	if (bIsDialogueActive || bIsInventoryActive) return; // Modlardayken pan yok

	if (bIsCameraLocked && !PanInput.IsZero()) bIsCameraLocked = false;

	if (CameraBoom && !PanInput.IsZero())
	{
		FRotator CamRot = CameraBoom->GetComponentRotation();
		FRotator YawRot(0, CamRot.Yaw, 0);
		FVector ForwardDir = YawRot.RotateVector(FVector::ForwardVector);
		FVector RightDir = YawRot.RotateVector(FVector::RightVector);
		FVector Movement = (ForwardDir * PanInput.X) + (RightDir * PanInput.Y);
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		TargetLocation += Movement * PanSpeed * DeltaTime;
	}
}

void ACRPG_CameraRig::ToggleCameraLock()
{
	if (bIsDialogueActive || bIsInventoryActive) return;
	bIsCameraLocked = !bIsCameraLocked;
	if (bIsCameraLocked && TargetActor) TargetLocation = TargetActor->GetActorLocation();
}

void ACRPG_CameraRig::AddYawInput(float Val)
{
	if (Val == 0.0f) return;
	TargetYaw += Val * 45.0f;
}

void ACRPG_CameraRig::AddZoomInput(float Val)
{
	if (bIsDialogueActive || bIsInventoryActive) return;
	if (Val == 0.0f) return;
	TargetArmLength = FMath::Clamp(TargetArmLength + (Val * ZoomSpeed * -1.0f), MinZoom, MaxZoom);
}