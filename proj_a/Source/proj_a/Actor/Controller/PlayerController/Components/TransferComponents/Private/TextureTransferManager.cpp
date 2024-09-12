#include "TextureTransferManager.h"

#include "EngineUtils.h"
#include "FNetLogger.h"

void UTextureTransferManager::RequestTextureToServer_Implementation(int32 PlayerId)
{
	ADPCharacter* Character = FindCharacterByPlayerId(PlayerId);
	if (!Character)
	{
		RetryRequestTexture(PlayerId);
		return;
	}

	UDynamicTextureComponent* DynamicTextureComp = Character->GetDynamicTextureComponent();
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

	SendLargeDataInChunks(
		Character->SerializeTexture(CustomTexture).CompressedTextureData,
		PlayerId);
}

void UTextureTransferManager::RetryRequestTexture(int32 PlayerId)
{
	FNetLogger::EditerLog(FColor::Red, TEXT("RetryRequestTexture PlayerId: %d"), PlayerId);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, PlayerId]() { RequestTextureToServer(PlayerId); });
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
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
		if (!dynamicTextureComp)
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([this, Key]() { OnTextureTransferComplete(Key); });
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
			return;
		}
		dynamicTextureComp->bCustomTextureUploaded = true;
		C->OnTransferComplete(ReceivedData);
		RemoveDataTransferInfo(Key);
	}
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
