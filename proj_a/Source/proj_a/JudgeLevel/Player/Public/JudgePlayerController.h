#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JudgeLevelUI.h"
#include "DelayedExecutionSystem.h"
#include "JudgePlayerController.generated.h"

enum EOccupationType : uint8
{
	
};

UCLASS()
class PROJ_A_API AJudgePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AJudgePlayerController();
	~AJudgePlayerController();

	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	UFUNCTION(Client, Reliable)
	void InitConstUI(const TArray<FString>& Ids, const TArray<int32>& Scores);

	// UFUNCTION(Client, Reliable)
	// void SetUiVoteCandidateName(const FString& Name);
	//
	// UFUNCTION(Client, Reliable)
	// void SetUiConfirmedVoteOccupation(EOccupationType Occupation);
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void NotifyTimerEnd();
	//
	// UFUNCTION(Server, Reliable)
	// void ReturnVoteResult();

	// void WaitForVoteResults();

	UJudgeLevelUI* GetJudgeLevelUI() const { return JudgeLevelUI; }
protected:
	virtual void BeginPlay() override;
	virtual void SeamlessTravelFrom(APlayerController* OldPC) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJudgeLevelUI> JudgeLevelUI_BP;
	UPROPERTY()
	UJudgeLevelUI* JudgeLevelUI;
	DelayedExecutionSystem* DES;
};
