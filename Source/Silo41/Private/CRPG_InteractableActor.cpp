#include "CRPG_InteractableActor.h"
#include "Components/PrimitiveComponent.h"
#include "CRPG_CharacterBase.h"       
#include "CRPG_InventoryComponent.h"  
#include "CRPG_LootComponent.h" // YENÝ: Loot Component Eriþimi 

ACRPG_InteractableActor::ACRPG_InteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	Name = FText::FromString("Unknown Object");
	ActionText = FText::FromString("Interact");
	bIsInteractable = true;

	// Loot defaults
	ItemToGive = nullptr;
	ItemQuantity = 1;
	bDestroyOnInteract = false;

	BaseMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BaseMesh->SetRenderCustomDepth(false);
	BaseMesh->SetCustomDepthStencilValue(1);
}

void ACRPG_InteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACRPG_InteractableActor::OnInteract_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("INTERACTION: Interaction triggered with %s"), *GetName());

	if (!InstigatorPawn) return;

	// Oyuncunun Envanterini Bul
	UCRPG_InventoryComponent* Inventory = InstigatorPawn->FindComponentByClass<UCRPG_InventoryComponent>();
	if (!Inventory) return;

	// --- YÖNTEM A: GELÝÞMÝÞ LOOT SÝSTEMÝ (LootComponent Varsa) ---
	UCRPG_LootComponent* LootComp = FindComponentByClass<UCRPG_LootComponent>();
	if (LootComp)
	{
		// Zarlarý at
		TArray<FLootResult> Loot = LootComp->RollLoot();

		if (Loot.Num() > 0)
		{
			// Çýkanlarý envantere ekle
			for (const FLootResult& Drop : Loot)
			{
				Inventory->AddItem(Drop.Item, Drop.Count);
			}

			// Sandýksa içini boþalt (Tekrar alýnamasýn mesajý)
			// LootComponent kendi içinde bLooted kontrolü yapýyor zaten.
			ActionText = FText::FromString("Empty");
		}
		else
		{
			// Zar atýldý ama þanssýzlýk sonucu boþ geldi veya zaten lootlanmýþtý
			UE_LOG(LogTemp, Log, TEXT("LOOT: Container is empty or nothing dropped."));
		}

		// LootComponent kullandýysak, aþaðýdaki eski basit sistemi es geç.
		return;
	}

	// --- YÖNTEM B: BASÝT LOOT SÝSTEMÝ (Eski usül, Component yoksa) ---
	if (ItemToGive)
	{
		bool bSuccess = Inventory->AddItem(ItemToGive, ItemQuantity);

		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("LOOT: Gave %d x %s to player."), ItemQuantity, *ItemToGive->Name.ToString());

			if (bDestroyOnInteract)
			{
				Destroy();
			}
			else
			{
				ItemToGive = nullptr;
				ActionText = FText::FromString("Empty");
			}
		}
	}
}

void ACRPG_InteractableActor::SetHighlight(bool bIsActive)
{
	TArray<UPrimitiveComponent*> AllComponents;
	GetComponents<UPrimitiveComponent>(AllComponents);

	for (UPrimitiveComponent* Comp : AllComponents)
	{
		if (Comp)
		{
			Comp->SetRenderCustomDepth(bIsActive);
			if (bIsActive)
			{
				Comp->SetCustomDepthStencilValue(1);
			}
		}
	}
}