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
	uint8* TextureData;
	UPROPERTY()
	UTexture2D* DynamicTexture;
	UPROPERTY()
	UTexture2D* BC6HTexture;
	FUpdateTextureRegion2D* TextureRegion;

public:
	UTexture2D* GetDynamicTexture() const { return DynamicTexture; }
	void InitializeTexture();
	void FillTexture(FLinearColor Color);
	void UpdateTexture(UTexture2D* inTexture, bool bFreeData = false);
	bool LoadTextureFromFile(const FString& FilePath);
	~UDynamicTextureComponent();

	UPROPERTY(EditDefaultsOnly)
	int32 TextureWidth = 1024;
	UPROPERTY(EditDefaultsOnly)
	int32 TextureHeight = 1024;
};
