#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataTransferManager.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDataTransferComplete, const int32, Key);

USTRUCT()
struct FDataChunkInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ChunkIndex = 0;
	UPROPERTY()
	int32 TotalChunks = 0;
	UPROPERTY()
	TArray<uint8> DataChunk = {};
	UPROPERTY()
	int32 Key = 0;
	UPROPERTY()
	int32 TransferID = 0;

	FDataChunkInfo() = default;
	FDataChunkInfo(int32 InChunkIndex, int32 InTotalChunks, const TArray<uint8>& InDataChunk, int32 InKey, int32 InTransferID)
		: ChunkIndex(InChunkIndex), TotalChunks(InTotalChunks), DataChunk(InDataChunk), Key(InKey), TransferID(InTransferID) {}
};

USTRUCT()
struct FDataTransferInfo
{
	GENERATED_BODY()

	// Array to store received data chunks
	TArray<uint8> ReceivedDataChunks;
	bool bDataTransferComplete = false;
	int32 TransferID = 0;
};

USTRUCT()
struct FDataTransferParams
{
	GENERATED_BODY()

	TArray<uint8> Data;
	int32 PlayerId;
	int32 ChunkSize;
	int32 TotalChunks;
	int32 StartChunk;
	int32 ChunksPerTick;
	int32 TransferID = 0;

	enum ETransferConstants
	{
		CHUNK_SIZE = 1024,
		CHUNK_PER_TICK = 8
	};
	
	FDataTransferParams()
		: PlayerId(0), ChunkSize(CHUNK_SIZE), TotalChunks(0), StartChunk(0), ChunksPerTick(CHUNK_PER_TICK)
	{}
	FDataTransferParams(const TArray<uint8>& InData, int32 InPlayerId)
		: Data(InData), PlayerId(InPlayerId)
	{
		ChunkSize = CHUNK_SIZE;
		TotalChunks = FMath::CeilToInt(static_cast<float>(Data.Num()) / ChunkSize);
		StartChunk = 0;
		ChunksPerTick = CHUNK_PER_TICK;
	}
};

UCLASS()
class UDataTransferManager : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnDataTransferComplete OnDataTransferComplete;
	void RemoveDataTransferInfo(int32 Key) { DataTransferMap.Remove(Key); }
	void SendLargeDataInChunks(FDataTransferParams Params);
	
	UFUNCTION(Server, Reliable)
	void ReceiveDataChunkForServer(const FDataChunkInfo DataChunkInfo);
	UFUNCTION(Client, Reliable)
	void ReceiveDataChunkForClient(const FDataChunkInfo DataChunkInfo);

protected:
	void SendDataChunks(FDataTransferParams Params);
	void HandleReceivedData(const FDataChunkInfo& DataChunkInfo);
	void ResetData(int32 Key);
	bool CanSendDataWithoutOverflow(const TArray<uint8>& Data, UFunction* Function, const FDataChunkInfo& ChunkInfo);
	
	TMap<int32, FDataTransferInfo> DataTransferMap;
	int32 DataTransferID = 0;
};
