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

	FDataChunkInfo() = default;
	FDataChunkInfo(int32 InChunkIndex, int32 InTotalChunks, const TArray<uint8>& InDataChunk, int32 InKey)
		: ChunkIndex(InChunkIndex), TotalChunks(InTotalChunks), DataChunk(InDataChunk), Key(InKey) {}
};

USTRUCT()
struct FDataTransferInfo
{
	GENERATED_BODY()

	// Array to store received data chunks
	TArray<uint8> ReceivedDataChunks;
	bool bDataTransferComplete = false;
};

UCLASS()
class UDataTransferManager : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnDataTransferComplete OnDataTransferComplete;
	void RemoveDataTransferInfo(int32 Key) { DataTransferMap.Remove(Key); }
	void SendLargeDataInChunks(const TArray<uint8>& Data, int32 PlayerId, int32 ChunkSize = 1024);
	
	UFUNCTION(Server, Reliable)
	void ReceiveDataChunkForServer(const FDataChunkInfo DataChunkInfo);
	UFUNCTION(Client, Reliable)
	void ReceiveDataChunkForClient(const FDataChunkInfo DataChunkInfo);

protected:
	void HandleReceivedData(const FDataChunkInfo& DataChunkInfo);
	void ResetData(int32 Key);

	TMap<int32, FDataTransferInfo> DataTransferMap;
	int32 ExpectedTotalChunks = 0;
};
