#include "TextureTransferManager.h"

#include "EngineUtils.h"
#include "FNetLogger.h"

void UTextureTransferManager::RequestTextureToServer_Implementation(int32 PlayerId)
{
	ADPCharacter* C = FindCharacterByPlayerId(PlayerId);
	UDynamicTextureComponent* dynamicTextureComp = C->GetDynamicTextureComponent();
	if (dynamicTextureComp && dynamicTextureComp->bCustomTextureUploaded == true)
	{
		UTexture2D* CustomTexture = dynamicTextureComp->GetDynamicTexture();
		if (CustomTexture)
		{	
			SendLargeDataInChunks(
					C->SerializeTexture(CustomTexture).CompressedTextureData,
					PlayerId);
		}
	}
	else
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this, PlayerId]() { RequestTextureToServer(PlayerId); });
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}

void UTextureTransferManager::OnTextureTransferComplete(const int32 Key)
{
	FDataTransferInfo& Data = DataTransferMap.FindChecked(Key);
	TArray<uint8>& ReceivedData = Data.ReceivedDataChunks;
	if (Data.bDataTransferComplete == false)
		return ;

	ADPCharacter* C = FindCharacterByPlayerId(Key);
	if (C)
	{
		UDynamicTextureComponent* dynamicTextureComp = C->GetDynamicTextureComponent();
		if (dynamicTextureComp)
			dynamicTextureComp->bCustomTextureUploaded = true;
		C->OnTransferComplete(ReceivedData);
	}
	RemoveDataTransferInfo(Key);
}

ADPCharacter* UTextureTransferManager::FindCharacterByPlayerId(int32 PlayerId) const
{
	for (TActorIterator<ADPCharacter> It(GetWorld()); It; ++It)
	{
		ADPCharacter* C = *It;
		if (C && C->GetPlayerState())
		{
			if (C->GetPlayerState()->GetPlayerId() == PlayerId)
			{
				return Cast<ADPCharacter>(C);
			}
		}
	}
	return nullptr;
}
