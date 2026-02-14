#include "CRPG_InteractionComponent.h"
#include "CRPG_InteractableActor.h"
#include "CRPG_CharacterBase.h" // Karakterleri de tanýmalýyýz
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UCRPG_InteractionComponent::UCRPG_InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	HighlightRadius = 1500.0f;
}

void UCRPG_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCRPG_InteractionComponent::ToggleHighlightMode(bool bEnable)
{
	// KAPATMA
	if (!bEnable)
	{
		for (AActor* Actor : HighlightedActors)
		{
			if (!Actor) continue;

			// InteractableActor mu?
			if (ACRPG_InteractableActor* Item = Cast<ACRPG_InteractableActor>(Actor))
			{
				Item->SetHighlight(false);
			}
			// Karakter mi?
			else if (ACRPG_CharacterBase* Char = Cast<ACRPG_CharacterBase>(Actor))
			{
				Char->SetHighlight(false);
			}
		}
		HighlightedActors.Empty();
		return;
	}

	// AÇMA
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	FVector MyLoc = MyOwner->GetActorLocation();
	TArray<AActor*> Candidates;

	// 1. Cansýz Objeleri Bul
	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(this, ACRPG_InteractableActor::StaticClass(), FoundItems);
	Candidates.Append(FoundItems);

	// 2. Karakterleri Bul (NPC'ler ve Düþmanlar)
	TArray<AActor*> FoundChars;
	UGameplayStatics::GetAllActorsOfClass(this, ACRPG_CharacterBase::StaticClass(), FoundChars);
	Candidates.Append(FoundChars);

	UE_LOG(LogTemp, Log, TEXT("HIGHLIGHT: Scanning... Total Candidates: %d"), Candidates.Num());

	for (AActor* Actor : Candidates)
	{
		if (!Actor || Actor == MyOwner) continue;

		float Dist = FVector::Dist(MyLoc, Actor->GetActorLocation());
		if (Dist <= HighlightRadius)
		{
			// InteractableActor ise
			if (ACRPG_InteractableActor* Item = Cast<ACRPG_InteractableActor>(Actor))
			{
				Item->SetHighlight(true);
				HighlightedActors.AddUnique(Item);
			}
			// Karakter ise
			else if (ACRPG_CharacterBase* Char = Cast<ACRPG_CharacterBase>(Actor))
			{
				// Sadece bizden olmayanlarý highlight et (Kendimizi boyamayalým)
				if (!Char->IsPlayerCharacter())
				{
					Char->SetHighlight(true);
					HighlightedActors.AddUnique(Char);
				}
			}
		}
	}
}

void UCRPG_InteractionComponent::TryInteractAtLocation(FVector Location)
{
	// Logic PlayerController'da
}