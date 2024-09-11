#pragma once

#include "CoreMinimal.h"
#include "DataTransferManager.h"
#include "DPCharacter.h"
#include "TextureTransferManager.generated.h"

UCLASS()
class UTextureTransferManager : public UDataTransferManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void RequestTextureToServer(int32 PlayerId);
	UFUNCTION()
	void OnTextureTransferComplete(const int32 Key);
	ADPCharacter* FindCharacterByPlayerId(int32 PlayerId) const;
private:
	void RetryRequestTexture(int32 PlayerId);
};
