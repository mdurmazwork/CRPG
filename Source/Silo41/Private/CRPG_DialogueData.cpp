#include "CRPG_DialogueData.h"

FDialogueNode* UCRPG_DialogueData::GetNodeByID(int32 ID)
{
	for (FDialogueNode& Node : Nodes)
	{
		if (Node.NodeID == ID)
		{
			return &Node;
		}
	}
	return nullptr;
}

FPrimaryAssetId UCRPG_DialogueData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("DialogueData", GetFName());
}
// .