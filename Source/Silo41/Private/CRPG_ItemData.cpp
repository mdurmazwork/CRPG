#include "CRPG_ItemData.h"

FPrimaryAssetId UCRPG_ItemData::GetPrimaryAssetId() const
{
	// Bu fonksiyon, DataAsset'in diskteki adýný deðil, bizim belirlediðimiz ID'yi kullanmasýný saðlar.
	// Böylece dosya adýný deðiþtirseniz bile oyun içi referanslar kopmaz (teorik olarak). 
	return FPrimaryAssetId("ItemData", GetFName());
}