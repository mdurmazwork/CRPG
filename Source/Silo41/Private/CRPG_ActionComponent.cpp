#include "CRPG_ActionComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCRPG_ActionComponent::UCRPG_ActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxActionPoints = 4;
	CurrentActionPoints = MaxActionPoints;
}

void UCRPG_ActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Baþlangýçta AP tam olsun
	ResetActionPoints();
}

void UCRPG_ActionComponent::ResetActionPoints()
{
	CurrentActionPoints = MaxActionPoints;
	OnActionPointsChanged.Broadcast(CurrentActionPoints, MaxActionPoints);

	UE_LOG(LogTemp, Log, TEXT("ACTION: AP Reset for %s. Current AP: %d"), *GetOwner()->GetName(), CurrentActionPoints);
}

bool UCRPG_ActionComponent::ExecuteAbility(UCRPG_AbilityData* AbilityToUse, AActor* TargetActor)
{
	if (!AbilityToUse)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACTION FAIL: No ability data provided!"));
		return false;
	}

	// 1. AP Kontrolü
	if (CurrentActionPoints < AbilityToUse->APCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACTION FAIL: Not enough AP! Needed: %d, Has: %d"), AbilityToUse->APCost, CurrentActionPoints);
		return false;
	}

	// 2. Hedef Kontrolü (Alan etkili deðilse hedef þart)
	if (AbilityToUse->TargetType == ETargetType::Enemy && !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACTION FAIL: Ability requires a target!"));
		return false;
	}

	// 3. AP Düþ ve Ýþlemi Baþlat
	CurrentActionPoints -= AbilityToUse->APCost;
	OnActionPointsChanged.Broadcast(CurrentActionPoints, MaxActionPoints);

	ApplyAbilityEffects(AbilityToUse, TargetActor);

	return true;
}

void UCRPG_ActionComponent::ApplyAbilityEffects(UCRPG_AbilityData* Ability, AActor* Target)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	UE_LOG(LogTemp, Log, TEXT("COMBAT: %s used %s!"), *OwnerChar->GetName(), *Ability->AbilityName.ToString());

	// A. Rotasyon: Hedefe Dön
	if (Target)
	{
		FVector LookAt = Target->GetActorLocation();
		LookAt.Z = OwnerChar->GetActorLocation().Z; // Z eksenini bozma

		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(OwnerChar->GetActorLocation(), LookAt);
		OwnerChar->SetActorRotation(TargetRot);
	}

	// B. Animasyon
	if (Ability->CastAnimation)
	{
		OwnerChar->PlayAnimMontage(Ability->CastAnimation);
	}

	// C. Görsel Efekt (VFX)
	if (Ability->ImpactVFX && Target)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Ability->ImpactVFX, Target->GetActorLocation());
	}

	// D. Ses (SFX)
	if (Ability->ImpactSound && Target)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Ability->ImpactSound, Target->GetActorLocation());
	}

	// E. Hasar (Logic)
	if (Target && Ability->DamageAmount > 0.0f)
	{
		// Unreal'ýn standart hasar sistemini kullanýyoruz.
		// Bu, hedefteki "TakeDamage" fonksiyonunu tetikler (AttributeComponent orada devreye girecek).
		UGameplayStatics::ApplyDamage(Target, Ability->DamageAmount, OwnerChar->GetController(), OwnerChar, UDamageType::StaticClass());
	}
}