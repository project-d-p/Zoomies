#include "TextureTransferManager.h"

#include "EngineUtils.h"
#include "FNetLogger.h"

void UTextureTransferManager::RequestTextureToServer_Implementation(int32 PlayerId)
{
	ADynamicTexturedCharacter* Character = FindCharacterByPlayerId(PlayerId);
	if (!Character)
	{
		RetryRequestTexture(PlayerId);
		return;
	}
	
	UNetworkedDynamicTextureComponent* DynamicTextureComp = Character->GetDynamicTextureComponent();
	if (!DynamicTextureComp || !DynamicTextureComp->bCustomTextureUploaded)
	{
		RetryRequestTexture(PlayerId);
		return;
	}
	
	UTexture2D* CustomTexture = DynamicTextureComp->GetDynamicTexture();
	if (!CustomTexture)
	{
		// RetryRequestTexture(PlayerId);
		return;
	}

	FDataTransferParams Params(DynamicTextureComp->SerializeTexture(CustomTexture).CompressedTextureData, PlayerId);
	SendLargeDataInChunks(Params);
}

void UTextureTransferManager::RetryRequestTexture(int32 PlayerId)
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, PlayerId]() { RequestTextureToServer(PlayerId); });
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}

void UTextureTransferManager::OnTextureTransferComplete(const int32 Key)
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Texture transfer complete for player %d"), Key);
	FDataTransferInfo& Data = DataTransferMap.FindChecked(Key);
	TArray<uint8>& ReceivedData = Data.ReceivedDataChunks;
	if (Data.bDataTransferComplete == false)
		return ;

	ADynamicTexturedCharacter* C = FindCharacterByPlayerId(Key);
	if (C)
	{
		UNetworkedDynamicTextureComponent* dynamicTextureComp = C->GetDynamicTextureComponent();
		if (!dynamicTextureComp)
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([this, Key]() { OnTextureTransferComplete(Key); });
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
			return;
		}
		dynamicTextureComp->bCustomTextureUploaded = true;
		dynamicTextureComp->OnTransferComplete(ReceivedData);
		RemoveDataTransferInfo(Key);
	}
}

ADynamicTexturedCharacter* UTextureTransferManager::FindCharacterByPlayerId(int32 PlayerId) const
{
	for (TActorIterator<ADynamicTexturedCharacter> It(GetWorld()); It; ++It)
	{
		ADynamicTexturedCharacter* C = *It;
		if (C && C->GetPlayerState())
		{
			if (C->GetPlayerState()->GetPlayerId() == PlayerId)
			{
				return Cast<ADynamicTexturedCharacter>(C);
			}
		}
	}
	return nullptr;
}
