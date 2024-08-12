#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NameTag.generated.h"

UCLASS()
class UNameTag : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
	void SetName(const FString& String);
protected:
	virtual void NativeConstruct() override;
	
};
