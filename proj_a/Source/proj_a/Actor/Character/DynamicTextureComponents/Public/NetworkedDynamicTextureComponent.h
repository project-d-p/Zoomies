#pragma once

#include "DynamicTextureComponent.h"
#include "NetworkedDynamicTextureComponent.generated.h"

class UTextureTransferManager;

USTRUCT()
struct FNetworkedDynamicTextureComponentInitializer
{
	GENERATED_BODY()
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	UPROPERTY()
	APlayerState* PlayerState = nullptr;
	UPROPERTY()
	UTextureTransferManager* TextureTransferManager = nullptr;
};

USTRUCT()
struct FSerializedTextureData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<uint8> CompressedTextureData;
	UPROPERTY()
	int32 Width = 0;
	UPROPERTY()
	int32 Height = 0;
};

UCLASS()
class UNetworkedDynamicTextureComponent : public UDynamicTextureComponent
{
	GENERATED_BODY()
public:
	UNetworkedDynamicTextureComponent() = default;

	bool InitializeTexture(FNetworkedDynamicTextureComponentInitializer& Initializer);
	FSerializedTextureData SerializeTexture(UTexture2D* Texture);
	void OnTransferComplete(const TArray<uint8>& FullData);
	void LoadTexture();

private:
	void HandleLocalNetOwner(UTexture2D* CustomTexture);
	void HandleRemoteNetOwner();
	UTexture2D* DeserializeTexture(FSerializedTextureData& InData);
	void UpdateTexture(UTexture2D* NewTexture);

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY()
	APlayerState* PlayerState;
	UPROPERTY()
	UTextureTransferManager* TextureTransferManager;
};
