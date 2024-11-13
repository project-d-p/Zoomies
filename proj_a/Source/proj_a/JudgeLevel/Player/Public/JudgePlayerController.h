#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JudgeLevelUI.h"
#include "JudgeGameMode.h"
#include "EnumTypes.h"
#include "TextureTransferManager.h"
#include "JudgePlayerController.generated.h"

class UJudgeLevelComponent;

// Enum for player jobs
inline FString OccupationToString(EPlayerJob Occupation)
{
	switch(Occupation)
	{
	case EPlayerJob::JOB_ARCHAEOLOGIST:
		return FString("ARCHAEOLOGIST");
	case EPlayerJob::JOB_POACHER:
		return FString("POACHER");
	case EPlayerJob::JOB_ENVIRONMENTALIST:
		return FString("ENVIRONMENTALIST");
	case EPlayerJob::JOB_RINGMASTER:
		return FString("RINGMASTER");
	case EPlayerJob::JOB_TERRORIST:
		return FString("TERRORIST");
	case EPlayerJob::JOB_CHECK:
		return FString("CHECK");
	case EPlayerJob::JOB_CROSS:
		return FString("CROSS");
	default:
		return FString("UNKNOWN");
	}
}

UCLASS()
class PROJ_A_API AJudgePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AJudgePlayerController();
	
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

	virtual void GetSeamlessTravelActorList(bool bToTransitionMap, TArray<AActor*>& ActorList) override;
	
	void ShowUI_ESC();

	void ActivateCurrentComponent(AJudgePlayerController* LocalPlayerController);
	void DeactivateCurrentComponent();

	UFUNCTION(Server, Reliable)
	void RequestCharacter();

	UJudgeLevelUI* GetJudgeLevelUI() const { return JudgeLevelUI; }
	AActor* CameraActor = nullptr;
protected:
	// 
	UFUNCTION()
	void OnPossessEvent(APawn* OldPawn, APawn* NewPawn);
	virtual void BeginPlay() override;
	virtual void SeamlessTravelFrom(APlayerController* OldPC) override;
	virtual void SeamlessTravelTo(APlayerController* NewPC) override;
	virtual void PostSeamlessTravel() override;
private:
	bool IsBeginPlay = false;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJudgeLevelUI> JudgeLevelUI_BP;
	UPROPERTY()
	UJudgeLevelUI* JudgeLevelUI;
	UPROPERTY()
	UTextureTransferManager* TextureTransferManager;

	USoundBase *TurnStartSound;
	UJudgeLevelComponent* LevelComponent = nullptr;
	void findMyCamera();

	FTimerHandle TH;
	FTimerHandle CTH;
};
