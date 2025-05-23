#pragma once

#include "CoreMinimal.h"
#include "DataManager.h"
#include "PlayerScoreComp.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "PlayerScoreData.h"
#include "DPPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJobChanged);

UCLASS()
class PROJ_A_API ADPPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADPPlayerState();
	
	UFUNCTION()
	UPlayerScoreComp* GetPlayerScoreComp() const;
	UPlayerScoreData* GetPlayerScoreData() const { return PlayerScoreData; }

	UFUNCTION()
	EPlayerJob GetPlayerJob() const;

	void SetPlayerRandomJob();
	void SetFinalScoreData(const FFinalScoreData& InFinalScoreData) { FinalScoreData = InFinalScoreData; }
	const FFinalScoreData& GetFinalScoreData() const { return FinalScoreData; }
	void SetPlayerScoreData(UPlayerScoreData* InPlayerScoreData);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerRank")
	int Rank;

	UFUNCTION(Server, Reliable)
	void ServerSetRank(int InRank);

	UPROPERTY(ReplicatedUsing=OnRep_PlayerJob, BlueprintReadWrite, Category = "PlayerJob")
	EPlayerJob PlayerJob;
	UFUNCTION()
	void OnRep_PlayerJob();
	
	void IncreaseScore(const TArray<EAnimal>& InAnimals);

	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

	/* Register & Unregister a player with the online subsystem */
	virtual void RegisterPlayerWithSession(bool bWasFromInvite) override;
	virtual void UnregisterPlayerWithSession() override;

	/*
	 * Call RegisterPlayerWithSession for all players (escpecially for host) during seamless travel
	 * Becuase OnRep_UniqueID() is not calling for host during Seamless Travel
	 */ 
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnSetUniqueId() override;
private:
	// Will Delete
	UPROPERTY()
	UPlayerScoreComp* PlayerScoreComp = nullptr;
	FFinalScoreData FinalScoreData;
	//

	void OnHostMigration(UWorld* World, UDataManager* DataManager);
	void AddInGameWidgetFunctionToDelegate();
	void InitializePlayerState();
	void SetPlayerNameDelayed();
	void SetSessionName();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY()
	UPlayerScoreData* PlayerScoreData = nullptr;
	FDelegateHandle OnHostMigrationDelegate;
	FOnJobChanged OnJobChanged;
	FTimerHandle PlayerNameTimerHandle;

	/* For Seamless Travel */
	bool SeamlessTraviling;
	bool bResponsedMyInfo;
};
