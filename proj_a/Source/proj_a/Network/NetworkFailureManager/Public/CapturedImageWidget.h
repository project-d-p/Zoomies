#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "CapturedImageWidget.generated.h"

UCLASS()
class UCapturedImageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* CapturedImage;

	void SetCapturedTexture(UTexture2D* Texture);
};

