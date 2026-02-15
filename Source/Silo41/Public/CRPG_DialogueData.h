#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CRPG_DialogueData.generated.h"

// Oyuncunun verebileceði cevap þýkký
USTRUCT(BlueprintType)
struct FDialogueOption
{
	GENERATED_BODY()

	// Þýkkýn metni (Örn: "Seni kim gönderdi?")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Text;

	// Bu þýkký seçince hangi ID'li düðüme gidilecek? (-1: Konuþmayý bitir) 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NextNodeID = -1;

	// Þýkkýn seçilmesi için gereken koþul (Opsiyonel)
	// Örn: "Has_Keycard_Blue"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ConditionTag;

	// Þýk seçilince tetiklenecek olay (Opsiyonel)
	// Örn: "Start_Quest_01" veya "Trade_Open"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ActionTag;
};

// NPC'nin tek bir konuþma balonunu temsil eden düðüm
USTRUCT(BlueprintType)
struct FDialogueNode
{
	GENERATED_BODY()

	// Düðüm ID'si (Benzersiz olmalý, örn: 0, 10, 20)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NodeID = 0;

	// Konuþan kiþinin adý (Boþ býrakýlýrsa NPC'nin kendi adý kullanýlýr)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SpeakerName;

	// NPC'nin söylediði metin
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true))
	FText Text;

	// Ses dosyasý (Opsiyonel)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* AudioLine;

	// Oyuncunun seçebileceði cevaplar
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDialogueOption> PlayerOptions;
};

/**
 * UCRPG_DialogueData
 * * KONUÞMA AÐACI
 * Görevi: Bir NPC ile yapýlan tüm sohbetin veri yapýsýný tutar.
 * DialogueWidget (UI) bu veriyi okuyarak ekrana basar.
 */
UCLASS(BlueprintType)
class SILO41_API UCRPG_DialogueData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- KÝMLÝK ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName DialogueID;

	// --- ÝÇERÝK ---

	// Tüm konuþma düðümleri (Sýrasýz olabilir, ID ile bulunur)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Content")
	TArray<FDialogueNode> Nodes;

	// Baþlangýç düðümü hangisi? (Genelde 0)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Content")
	int32 StartNodeID = 0;

	// Yardýmcý Fonksiyon: ID'ye göre düðümü getir
	FDialogueNode* GetNodeByID(int32 ID);

	// Asset Manager ID Override
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};