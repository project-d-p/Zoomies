#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JudgeLevelUI.h"
#include "JudgePlayerController.generated.h"

UCLASS()
class PROJ_A_API AJudgePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AJudgePlayerController();

	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	UJudgeLevelUI* GetJudgeLevelUI() const { return JudgeLevelUI; }
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJudgeLevelUI> JudgeLevelUI_BP;
	UPROPERTY()
	UJudgeLevelUI* JudgeLevelUI;
};
