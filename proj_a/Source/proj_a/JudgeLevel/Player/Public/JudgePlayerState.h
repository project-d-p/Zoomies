#pragma once

#include "CoreMinimal.h"
#include "DataManager.h"
#include "PlayerScoreData.h"
#include "GameFramework/PlayerState.h"
#include "ScoreTypes.h"
#include "JudgePlayerState.generated.h"

UCLASS()
class PROJ_A_API AJudgePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AJudgePlayerState();
	void SetCapturedAnimals(const TArray<TArray<EAnimal>>& CapturedAnimals) { FinalScoreData.CapturedAnimals = CapturedAnimals; }
	void SetScoreDatas(const TArray<FScoreData>& ScoreDatas) { FinalScoreData.ScoreDatas = ScoreDatas; }
	void SetIsDetected(bool bIsDetected) { if(PlayerScoreData) {PlayerScoreData->SetIsDetected(bIsDetected);} }
	FFinalScoreData GetFinalScoreData() const { return FinalScoreData; }
	UPlayerScoreData* GetPlayerScoreData() const { return PlayerScoreData; }
	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

	void SetPlayerJob(EPlayerJob InPlayerJob) { PlayerJob = InPlayerJob; }
	EPlayerJob GetPlayerJob() const { return PlayerJob; }
	void SetPlayerScoreData(UPlayerScoreData* InPlayerScoreData);

	/* Register and Unregister a player with the online subsystem */
	virtual void RegisterPlayerWithSession(bool bWasFromInvite) override;
	virtual void UnregisterPlayerWithSession() override;

	/*
	 * Call RegisterPlayerWithSession for all players (escpecially for host) during seamless travel
	 * Becuase OnRep_UniqueID() is not calling for host during Seamless Travel
	 */ 
	virtual void CopyProperties(APlayerState* PlayerState) override;

protected:
	void SetSessionName();
	void OnHostMigration(UWorld* World, UDataManager* DataManager);

	void SetPlayerNameDelayed();
	void InitializePlayerState();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(Replicated)
	UPlayerScoreData* PlayerScoreData;
	FFinalScoreData FinalScoreData;
	EPlayerJob PlayerJob;
	bool SeamlessTravling;
	FDelegateHandle OnHostMigrationDelegate;
	FTimerHandle PlayerNameTimerHandle;
};
