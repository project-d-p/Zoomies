#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "ChatUI.h"
#include "MyUserWidget.generated.h"

UCLASS()
class PROJ_A_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnScoreBT1Clicked();
	UFUNCTION()
	void OnScoreBT2Clicked();
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ScoreBT1;
	UPROPERTY(meta = (BindWidget))
	class UButton* ScoreBT2;
};
