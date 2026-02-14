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

	// Kamera kolu dünya rotasyonunu kullansýn (Böylece Actor dönse bile kamera dönmez, biz elle döndürürüz)
	CameraBoom->SetAbsolute(false, true, false);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false; // Ýzometrikte kamera duvara çarpýp zýplamasýn
	CameraBoom->TargetArmLength = 1400.0f;

	// Baþlangýç açýsý (Klasik RPG açýsý)
	CameraBoom->SetWorldRotation(FRotator(-45.0f, -45.0f, 0.0f));

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	MainCamera->bUsePawnControlRotation = false;

	// Baþlangýç deðerleri
	TargetYaw = -45.0f;
	TargetArmLength = 1400.0f;
	bIsCameraLocked = true; // Oyuna karakteri takip ederek baþla
}

void ACRPG_CameraRig::BeginPlay()
{
	Super::BeginPlay();

	TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);

	// Baþlangýçta kamerayý oyuncunun tepesine ýþýnla
	if (TargetActor)
	{
		SetActorLocation(TargetActor->GetActorLocation());
		TargetLocation = GetActorLocation();
	}

	if (CameraBoom)
	{
		TargetYaw = CameraBoom->GetComponentRotation().Yaw;
	}
}

void ACRPG_CameraRig::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. POZÝSYON MANTIÐI
	if (bIsCameraLocked && TargetActor)
	{
		// Kilitliyse: Hedefimiz karakterin pozisyonudur.
		TargetLocation = TargetActor->GetActorLocation();
	}
	// Kilitli deðilse: TargetLocation, AddPanInput fonksiyonu tarafýndan güncelleniyor.

	FVector CurrentLoc = GetActorLocation();
	// VInterpTo ile yumuþak geçiþ saðla
	FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLocation, DeltaTime, CameraSmoothSpeed);
	SetActorLocation(NewLoc);


	// 2. ROTASYON MANTIÐI (Sadece SpringArm döner)
	if (CameraBoom)
	{
		FRotator CurrentRot = CameraBoom->GetComponentRotation();
		FRotator TargetRot = FRotator(-45.0f, TargetYaw, 0.0f); // Pitch sabit

		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSmoothSpeed);
		CameraBoom->SetWorldRotation(NewRot);
	}

	// 3. ZOOM MANTIÐI
	if (CameraBoom)
	{
		float CurrentLen = CameraBoom->TargetArmLength;
		float NewLen = FMath::FInterpTo(CurrentLen, TargetArmLength, DeltaTime, CameraSmoothSpeed);
		CameraBoom->TargetArmLength = NewLen;
	}
}

void ACRPG_CameraRig::AddPanInput(const FVector2D& PanInput)
{
	// Eðer kilitliyse, oyuncu hareket etmeye çalýþtýðýnda kilidi kýr.
	if (bIsCameraLocked && !PanInput.IsZero())
	{
		bIsCameraLocked = false;
	}

	if (CameraBoom && !PanInput.IsZero())
	{
		// Pan yönünü kameranýn baktýðý yöne (Yaw) göre ayarla.
		// W'ye basýnca kameranýn baktýðý yere gitmeli, Dünya'nýn kuzeyine deðil.
		FRotator CamRot = CameraBoom->GetComponentRotation();
		FRotator YawRot(0, CamRot.Yaw, 0);

		// Input X (Forward), Input Y (Right)
		FVector ForwardDir = YawRot.RotateVector(FVector::ForwardVector);
		FVector RightDir = YawRot.RotateVector(FVector::RightVector);

		FVector Movement = (ForwardDir * PanInput.X) + (RightDir * PanInput.Y);

		// Hedef pozisyonu güncelle
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		TargetLocation += Movement * PanSpeed * DeltaTime;
	}
}

void ACRPG_CameraRig::ToggleCameraLock()
{
	bIsCameraLocked = !bIsCameraLocked;

	// Kilitlendiði an hedef pozisyonu hemen oyuncuya çek ki lerp oraya gitmeye baþlasýn
	if (bIsCameraLocked && TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}
}

void ACRPG_CameraRig::AddYawInput(float Val)
{
	if (Val == 0.0f) return;
	TargetYaw += Val * 45.0f;
}

void ACRPG_CameraRig::AddZoomInput(float Val)
{
	if (Val == 0.0f) return;
	TargetArmLength = FMath::Clamp(TargetArmLength + (Val * ZoomSpeed * -1.0f), MinZoom, MaxZoom);
}