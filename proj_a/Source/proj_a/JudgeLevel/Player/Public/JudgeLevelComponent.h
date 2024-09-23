#pragma once

#include "BaseLevelComponent.h"
#include "JudgeLevelUI.h"
#include "JudgeLevelComponent.generated.h"

// Enum for player jobs
// inline FString OccupationToString(EPlayerJob Occupation)
// {
// 	switch(Occupation)
// 	{
// 	case EPlayerJob::JOB_ARCHAEOLOGIST:
// 		return FString("ARCHAEOLOGIST");
// 	case EPlayerJob::JOB_POACHER:
// 		return FString("POACHER");
// 	case EPlayerJob::JOB_ENVIRONMENTALIST:
// 		return FString("ENVIRONMENTALIST");
// 	case EPlayerJob::JOB_RINGMASTER:
// 		return FString("RINGMASTER");
// 	case EPlayerJob::JOB_TERRORIST:
// 		return FString("TERRORIST");
// 	case EPlayerJob::JOB_CHECK:
// 		return FString("CHECK");
// 	case EPlayerJob::JOB_CROSS:
// 		return FString("CROSS");
// 	default:
// 		return FString("UNKNOWN");
// 	}
// }

UCLASS()
class PROJ_A_API UJudgeLevelComponent : public UBaseLevelComponent
{
	GENERATED_BODY()
public:
	UJudgeLevelComponent();
	
	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	UFUNCTION(Client, Reliable)
	void SetOccupationeName(int index, const FString& Name);

	UFUNCTION(Client, Reliable)
	void SetVoterName(const FString& Name);

	UFUNCTION(Client, Reliable)
	void InitializeUI(const FUIInitData UIData);
	
	UFUNCTION(Server, Reliable)
	void ReturnVote(EPlayerJob Type);

	UFUNCTION(Server, Reliable)
	void RequestUIData();

	UJudgeLevelUI* GetJudgeLevelUI() const { return JudgeLevelUI; }
protected:
	virtual void BeginPlay() override;
	
private:
	bool IsBeginPlay = false;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJudgeLevelUI> JudgeLevelUI_BP;

	UPROPERTY()
	UJudgeLevelUI* JudgeLevelUI;

	FTimerHandle TH;
	
	ADPPlayerController* PC = nullptr;
};
