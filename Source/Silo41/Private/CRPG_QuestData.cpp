#include "CRPG_QuestData.h"

FPrimaryAssetId UCRPG_QuestData::GetPrimaryAssetId() const
{
	// Asset Manager'ýn bu veriyi ID ile bulabilmesi için
	return FPrimaryAssetId("QuestData", GetFName());
}