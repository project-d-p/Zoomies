#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JudgeLevelUI.h"
#include "EnumTypes.h"
#include "DelayedExecutionSystem.h"
#include "JudgePlayerController.generated.h"

UCLASS()
class PROJ_A_API AJudgePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AJudgePlayerController();
	void InitConstUI();
	
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	UFUNCTION(Client, Reliable)
	void SetOccupationeName(int index, const FString& Name);
	
	UFUNCTION(NetMulticast, Reliable)
	void NotifyTimerEnd();
	
	UFUNCTION(Server, Reliable)
	void ReturnVote(EOccupation Type);

	UJudgeLevelUI* GetJudgeLevelUI() const { return JudgeLevelUI; }
protected:
	virtual void BeginPlay() override;
	virtual void SeamlessTravelFrom(APlayerController* OldPC) override;
private:
	bool IsBeginPlay = false;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJudgeLevelUI> JudgeLevelUI_BP;
	UPROPERTY()
	UJudgeLevelUI* JudgeLevelUI;
};
