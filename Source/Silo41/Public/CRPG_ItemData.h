#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CRPG_ItemData.generated.h"

// Eþya Türleri
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable UMETA(DisplayName = "Consumable"), // Tüketilebilir (Yemek, Ýlaç)
	Equipment  UMETA(DisplayName = "Equipment"),  // Giyilebilir (Silah, Zýrh)
	Quest      UMETA(DisplayName = "Quest Item"), // Görev Eþyasý (Anahtar, Belge)
	Junk       UMETA(DisplayName = "Junk")        // Çöp (Satýlacak hurda)
};

/**
 * UCRPG_ItemData
 * * EÞYA KÝMLÝÐÝ
 * Görevi: Envanterdeki bir eþyanýn tüm statik verilerini tutar.
 * InventoryComponent bu veriyi referans olarak saklar. 
 */
UCLASS(BlueprintType)
class SILO41_API UCRPG_ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- KÝMLÝK (IDENTITY) ---

	// Benzersiz ID (Örn: "Item_Medkit_Small")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName ItemID;

	// Ekranda görünecek isim (Örn: "Small Medkit")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FText Name;

	// Eþya açýklamasý
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity", meta = (MultiLine = true))
	FText Description;

	// --- GÖRSELLER (VISUALS) ---

	// Envanterdeki ikon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UTexture2D* Icon;

	// Yere atýldýðýnda görünecek model (Pickup Mesh)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	UStaticMesh* WorldMesh;

	// --- KURALLAR (RULES) ---

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	bool bIsStackable;

	// Maksimum kaç adet üst üste binebilir?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules", meta = (EditCondition = "bIsStackable"))
	int32 MaxStackSize = 99;

	// Eþyanýn aðýrlýðý (Survival mekaniði için)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	float Weight = 0.5f;

	// --- MANTIK ENJEKSÝYONU (LOGIC) ---

	/** * [CRITICAL] Data-Injection Anahtarý
	 * Bu eþya kullanýldýðýnda C++ tarafýnda hangi fonksiyon çalýþacak?
	 * Örn: "Heal_50", "Add_Ammo_30", "Unlock_Door_Blue"
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Logic")
	FName FunctionTag;

	// Asset Manager için ID Override (Asset Registry'de kolay bulmak için)
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};