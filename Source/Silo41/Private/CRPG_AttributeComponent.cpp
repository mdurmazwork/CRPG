#include "CRPG_AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UCRPG_AttributeComponent::UCRPG_AttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	BaseMovementSpeed = 600.0f;
}

void UCRPG_AttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCRPG_AttributeComponent::InitializeStats(const FMutationStats& BaseStats)
{
	MaxHealth = BaseStats.MaxHealth;
	CurrentHealth = MaxHealth;

	MaxStamina = BaseStats.MaxStamina;
	CurrentStamina = MaxStamina;

	BaseMovementSpeed = BaseStats.MovementSpeed;

	UE_LOG(LogTemp, Log, TEXT("SILO-41 SYSTEM: Attribute Component Initialized. HP: %f"), MaxHealth);
}

void UCRPG_AttributeComponent::ApplyHealthChange(float Delta)
{
	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealth);

	if (CurrentHealth != OldHealth)
	{
		UE_LOG(LogTemp, Log, TEXT("ATTRIBUTE: Health Changed. New HP: %f (Delta: %f)"), CurrentHealth, Delta);

		if (CurrentHealth <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATTRIBUTE: Character DIED!"));
			// TODO: Death Logic here
		}
	}
}

float UCRPG_AttributeComponent::GetHealthPercent() const
{
	return (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;
}

// --- STATUS EFFECT LOGIC ---

void UCRPG_AttributeComponent::AddStatusEffect(UCRPG_StatusEffect* NewEffect)
{
	if (!NewEffect) return;

	// Basit Mantýk: Ayný etki varsa süresini yenile, yoksa ekle.
	bool bFound = false;
	for (FActiveStatusEffect& ActiveEffect : ActiveEffects)
	{
		if (ActiveEffect.EffectData == NewEffect)
		{
			ActiveEffect.TurnsLeft = NewEffect->DurationTurns;
			UE_LOG(LogTemp, Log, TEXT("STATUS: Refreshed effect %s"), *NewEffect->EffectName.ToString());
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		FActiveStatusEffect NewActiveEffect;
		NewActiveEffect.EffectData = NewEffect;
		NewActiveEffect.TurnsLeft = NewEffect->DurationTurns;

		// Görsel Efekt Baþlat
		if (NewEffect->PersistentVFX)
		{
			NewActiveEffect.VFXInstance = UGameplayStatics::SpawnEmitterAttached(
				NewEffect->PersistentVFX,
				GetOwner()->GetRootComponent(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget
			);
		}

		ActiveEffects.Add(NewActiveEffect);
		UE_LOG(LogTemp, Warning, TEXT("STATUS: Applied %s to %s"), *NewEffect->EffectName.ToString(), *GetOwner()->GetName());
	}

	// Hýz deðiþmiþ olabilir, CharacterMovement'ý güncellemek gerekebilir (Tick'te alýyorsa sorun yok)
}

bool UCRPG_AttributeComponent::ProcessTurnStart()
{
	bool bIsStunned = false;

	// Ters döngü (Array'den eleman silerken güvenli yöntem)
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; i--)
	{
		FActiveStatusEffect& Effect = ActiveEffects[i];

		// 1. Etki Uygula
		if (Effect.EffectData->DamagePerTurn != 0.0f)
		{
			ApplyHealthChange(-Effect.EffectData->DamagePerTurn);
			UE_LOG(LogTemp, Log, TEXT("STATUS: %s took %f damage from %s"), *GetOwner()->GetName(), Effect.EffectData->DamagePerTurn, *Effect.EffectData->EffectName.ToString());
		}

		// 2. Kontrol Etkisi (Stun)
		if (Effect.EffectData->bStunsCharacter)
		{
			bIsStunned = true;
		}

		// 3. Süreyi Azalt
		Effect.TurnsLeft--;

		// 4. Bitti mi?
		if (Effect.TurnsLeft <= 0)
		{
			UE_LOG(LogTemp, Log, TEXT("STATUS: Effect %s expired."), *Effect.EffectData->EffectName.ToString());

			// VFX Temizle
			if (Effect.VFXInstance)
			{
				Effect.VFXInstance->DeactivateSystem();
				Effect.VFXInstance->DestroyComponent();
			}

			ActiveEffects.RemoveAt(i);
		}
	}

	return bIsStunned;
}

float UCRPG_AttributeComponent::GetMovementSpeed() const
{
	float FinalSpeed = BaseMovementSpeed;

	for (const FActiveStatusEffect& Effect : ActiveEffects)
	{
		if (Effect.EffectData)
		{
			FinalSpeed *= Effect.EffectData->SpeedMultiplier;
		}
	}

	return FinalSpeed;
}