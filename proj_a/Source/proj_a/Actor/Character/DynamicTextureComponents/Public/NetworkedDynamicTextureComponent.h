#pragma once

#include "DynamicTextureComponent.h"
#include "NetworkedDynamicTextureComponent.generated.h"

class UTextureTransferManager;

USTRUCT(BlueprintType)
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

USTRUCT(BlueprintType)
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

UCLASS(Blueprintable)
class UNetworkedDynamicTextureComponent : public UDynamicTextureComponent
{
	GENERATED_BODY()
public:
	UNetworkedDynamicTextureComponent() = default;

	UFUNCTION(BlueprintCallable, Category = "DynamicTexture")
	bool InitializeTexture(FNetworkedDynamicTextureComponentInitializer& Initializer);
	UFUNCTION(BlueprintCallable, Category = "DynamicTexture")
	FSerializedTextureData SerializeTexture(UTexture2D* Texture);
	UFUNCTION(BlueprintCallable, Category = "DynamicTexture")
	void LoadTexture();

	void OnTransferComplete(const TArray<uint8>& FullData);
private:
	void HandleLocalNetOwner(UTexture2D* CustomTexture);
	void HandleRemoteNetOwner();
	UTexture2D* DeserializeTexture(FSerializedTextureData& InData);
	void UpdateTexture(UTexture2D* NewTexture);

	UPROPERTY(VisibleAnywhere, Category = "DynamicTexture")
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	UPROPERTY(VisibleAnywhere, Category = "DynamicTexture")
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "DynamicTexture")
	APlayerState* PlayerState;
	UPROPERTY(VisibleAnywhere, Category = "DynamicTexture")
	UTextureTransferManager* TextureTransferManager;
};
