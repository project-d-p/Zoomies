#include "DataTransferManager.h"

#include "FNetLogger.h"

void UDataTransferManager::ReceiveDataChunkForClient_Implementation(const FDataChunkInfo DataChunkInfo)
{
	HandleReceivedData(DataChunkInfo);
}

void UDataTransferManager::ReceiveDataChunkForServer_Implementation(const FDataChunkInfo DataChunkInfo)
{
	HandleReceivedData(DataChunkInfo);
}

void UDataTransferManager::SendLargeDataInChunks(const TArray<uint8>& Data, int32 PlayerId, int32 ChunkSize)
{
	int32 TotalSize = Data.Num();
	int32 TotalChunks = FMath::CeilToInt(static_cast<float>(TotalSize) / ChunkSize);
	/*ExpectedTotalChunks = TotalChunks;*/

	for (int32 ChunkIndex = 0; ChunkIndex < TotalChunks; ++ChunkIndex)
	{
		int32 StartIndex = ChunkIndex * ChunkSize;
		int32 EndIndex = FMath::Min(StartIndex + ChunkSize, TotalSize);

		TArray<uint8> DataChunk;
		DataChunk.Append(&Data[StartIndex], EndIndex - StartIndex);
		if (GetOwner()->HasAuthority())
		{
			ReceiveDataChunkForClient(FDataChunkInfo(ChunkIndex, TotalChunks, DataChunk, PlayerId));
		}
		else
		{
			ReceiveDataChunkForServer(FDataChunkInfo(ChunkIndex, TotalChunks, DataChunk, PlayerId));	
		}
	}
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
