#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RHIFwd.h"
#include "DynamicTextureComponent.generated.h"

UCLASS()
class UDynamicTextureComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UTexture2D* DynamicTexture;
	FUpdateTextureRegion2D* TextureRegion;

public:
	uint8* TextureData;
	bool bCustomTextureUploaded = false;
	UTexture2D* GetDynamicTexture() const { return DynamicTexture; }
	void SetDynamicTexture(UTexture2D* inTexture) { DynamicTexture = inTexture; }
	void InitializeTexture();
	void FillTexture(FLinearColor Color);
	void UpdateTexture(UTexture2D* inTexture, uint8* InSrcData, bool bFreeData = false);
	bool LoadTextureFromFile(const FString& FilePath);
	TArray64<uint8> CompressTextureDataToEXR();
	TArray64<uint8> DecompressEXRToRawData(const TArray<uint8>& CompressedData, int32& OutWidth, int32& OutHeight);
	~UDynamicTextureComponent();

	UPROPERTY(EditDefaultsOnly)
	int32 TextureWidth = 1024;
	UPROPERTY(EditDefaultsOnly)
	int32 TextureHeight = 1024;
};
