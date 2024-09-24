#include "DataTransferManager.h"

#include "FNetLogger.h"
#include "NiagaraDataSetAccessor.h"
#include "Engine/ActorChannel.h"

void UDataTransferManager::ReceiveDataChunkForClient_Implementation(const FDataChunkInfo DataChunkInfo)
{
	HandleReceivedData(DataChunkInfo);
}

void UDataTransferManager::ReceiveDataChunkForServer_Implementation(const FDataChunkInfo DataChunkInfo)
{
	HandleReceivedData(DataChunkInfo);
}

void UDataTransferManager::SendLargeDataInChunks(FDataTransferParams Params)
{
	FNetLogger::EditerLog(FColor::Green, TEXT("SendLargeDataInChunks: %d"), Params.PlayerId);
	SendDataChunks(Params);
}

void UDataTransferManager::SendDataChunks(FDataTransferParams Params)
{
	const int32& StartChunk = Params.StartChunk;
	const int32& ChunksPerTick = Params.ChunksPerTick;
	const int32& TotalChunks = Params.TotalChunks;
	const int32& ChunkSize = Params.ChunkSize;
	const TArray<uint8>& Data = Params.Data;
	const int32& PlayerId = Params.PlayerId;

	int32 ChunksSentThisTick = 0;
	for (int32 ChunkIndex = StartChunk; ChunkIndex < TotalChunks; ++ChunkIndex)
	{
		int32 StartIndex = ChunkIndex * ChunkSize;
		int32 EndIndex = FMath::Min(StartIndex + ChunkSize, Data.Num());

		TArray<uint8> DataChunk;
		DataChunk.Append(&Data[StartIndex], EndIndex - StartIndex);

		FDataChunkInfo ChunkInfo(ChunkIndex, TotalChunks, DataChunk, PlayerId);
		UFunction* Function = nullptr;

		if (GetOwner()->HasAuthority())
		{
			Function = GetOwner()->FindFunction(TEXT("ReceiveDataChunkForClient"));
		}
		else
		{
			Function = GetOwner()->FindFunction(TEXT("ReceiveDataChunkForServer"));
		}

		if (ChunksSentThisTick++ >= ChunksPerTick || !CanSendDataWithoutOverflow(Data, Function, ChunkInfo))
		{
			Params.StartChunk = ChunkIndex;
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(
				this, &UDataTransferManager::SendDataChunks, Params));
			return;
		}

		if (GetOwner()->HasAuthority())
		{
			ReceiveDataChunkForClient(ChunkInfo);
		}
		else
		{
			ReceiveDataChunkForServer(ChunkInfo);
		}
	}
}

bool UDataTransferManager::CanSendDataWithoutOverflow(const TArray<uint8>& Data, UFunction* Function, const FDataChunkInfo& ChunkInfo)
{
	int32 TotalDataSize = Data.Num();
	UNetConnection* NetConnection = GetOwner()->GetNetConnection();
	if (!NetConnection) { return false; }

	int32 BunchSize = NetConnection->GetMaxSingleBunchSizeBits();
	int32 RequiredBunches = FMath::CeilToInt((float)TotalDataSize / BunchSize);

	if (NetConnection)
	{
		for (UChannel* Channel : NetConnection->OpenChannels)
		{
			UActorChannel* ActorChannel = Cast<UActorChannel>(Channel);
			if (ActorChannel)
			{
				FOutBunch TempBunch(ActorChannel, 0);
				
				int32 TotalRequiredSpace = RequiredBunches + ActorChannel->NumOutRec;

				if (TotalRequiredSpace < RELIABLE_BUFFER - 1)
				{
					return true;
				}
				FNetLogger::LogWarning(TEXT("Can't send data, Required Bunches: %d and NumOutRec: %d"), RequiredBunches, ActorChannel->NumOutRec);
				return false;
			}
		}
	}
	// can't find any actor channel to send data
	return false;
}

void UDataTransferManager::HandleReceivedData(const FDataChunkInfo& DataChunkInfo)
{
	const int32& ChunkIndex = DataChunkInfo.ChunkIndex;
	const int32& TotalChunks = DataChunkInfo.TotalChunks;
	const TArray<uint8>& DataChunk = DataChunkInfo.DataChunk;
	const int32& Key = DataChunkInfo.Key;

	FDataTransferInfo& Data = DataTransferMap.FindOrAdd(Key);
	TArray<uint8>& ReceivedData = Data.ReceivedDataChunks;
	
	if (/*TotalChunks != ExpectedTotalChunks ||*/ ChunkIndex == 0)
	{
		Data.bDataTransferComplete = false;
		ResetData(Key);
		// ExpectedTotalChunks = TotalChunks;
	}
	
	ReceivedData.Append(DataChunk);

	if (ChunkIndex == TotalChunks - 1)
	{
		Data.bDataTransferComplete = true;
		if (OnDataTransferComplete.IsBound())
		{
			OnDataTransferComplete.Execute(Key);
		}
	}
}

void UDataTransferManager::ResetData(int32 Key)
{
	DataTransferMap.Find(Key)->ReceivedDataChunks.Empty();
	ExpectedTotalChunks = 0;
}
