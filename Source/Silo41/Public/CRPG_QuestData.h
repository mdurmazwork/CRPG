#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CRPG_ItemData.h" // Ödül sistemi için item verisine ihtiyacýmýz var
#include "CRPG_QuestData.generated.h"

// Görev Türü
UENUM(BlueprintType)
enum class EQuestType : uint8
{
	MainStory   UMETA(DisplayName = "Main Story"),
	SideQuest   UMETA(DisplayName = "Side Quest"),
	Errand      UMETA(DisplayName = "Errand / Task")
};

// Görev Hedef Türü (Objective Type)
UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	Interact    UMETA(DisplayName = "Interact with Object"),
	KillTarget  UMETA(DisplayName = "Eliminate Target"),
	CollectItem UMETA(DisplayName = "Collect Item"),
	ReachZone   UMETA(DisplayName = "Reach Location")
};

// Görev Adýmý (Stage) Yapýsý
USTRUCT(BlueprintType)
struct FQuestStage
{
	GENERATED_BODY()

	// Bu adýmýn ID'si (Örn: 1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StageID = 1;

	// Oyuncuya gösterilecek talimat (Örn: "Silo Kapýsýný Bul")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	// Hedef Türü
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EQuestObjectiveType ObjectiveType;

	// Hedef Parametresi (Örn: KillTarget ise TargetID, CollectItem ise ItemID)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName TargetID;

	// Gerekli Miktar (Örn: 5 tane topla, 1 tane öldür)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredQuantity = 1;

	// Bu adým tamamlanýnca bir sonraki adým ne olacak? (-1 ise görev biter)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NextStageID = -1;
};

/**
 * UCRPG_QuestData
 * * HÝKAYE KARTI
 * Görevi: Bir görevin baþýný, sonunu, adýmlarýný ve ödüllerini tanýmlar.
 * QuestManager (henüz yazýlmadý) bu veriyi okuyarak görevi yönetir.
 */
UCLASS(BlueprintType)
class SILO41_API UCRPG_QuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- KÝMLÝK (IDENTITY) ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName QuestID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FText QuestTitle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity", meta = (MultiLine = true))
	FText QuestSummary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	EQuestType QuestType;

	// --- AKIÞ (FLOW) ---

	// Görevin adýmlarý (Sýralý liste yerine Map kullanabiliriz ama basitlik için Array)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flow")
	TArray<FQuestStage> Stages;

	// --- ÖDÜLLER (REWARDS) ---

	// Görev bitince verilecek XP (veya Geliþim Puaný)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewards")
	int32 ExperienceReward = 100;

	// Verilecek Eþya (Varsa)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewards")
	UCRPG_ItemData* RewardItem;

	// Asset Manager ID Override
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};