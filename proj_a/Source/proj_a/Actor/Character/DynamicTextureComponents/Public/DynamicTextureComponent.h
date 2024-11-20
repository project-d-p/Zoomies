#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RHIFwd.h"
#include "DynamicTextureComponent.generated.h"

UCLASS(Blueprintable)
class UDynamicTextureComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UTexture2D* DynamicTexture = nullptr;
	FUpdateTextureRegion2D* TextureRegion;

public:
	UDynamicTextureComponent() = default;
	virtual ~UDynamicTextureComponent() override;

	bool bCustomTextureUploaded = false;
	UTexture2D* GetDynamicTexture() const { return DynamicTexture; }
	
protected:
	uint8* TextureData;
	void SetDynamicTexture(UTexture2D* inTexture) { DynamicTexture = inTexture; }
	void InitializeTexture();
	void FillTexture(FLinearColor Color);
	void UpdateTexture(UTexture2D* inTexture, uint8* InSrcData, bool bFreeData = false);
	bool LoadTextureFromFile(const FString& FilePath);
	TArray64<uint8> CompressTextureDataToEXR();
	TArray64<uint8> DecompressEXRToRawData(const TArray<uint8>& CompressedData, int32& OutWidth, int32& OutHeight);

	UPROPERTY(EditDefaultsOnly)
	int32 TextureWidth = 1024;
	UPROPERTY(EditDefaultsOnly)
	int32 TextureHeight = 1024;
};
