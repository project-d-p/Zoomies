#pragma once

#include "CoreMinimal.h"
#include "DataManager.h"
#include "FNetLogger.h"
#include "NetworkFailureManager.h"
#include "PlayerScoreComp.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "PlayerScoreData.h"
#include "DPPlayerState.generated.h"

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
	void SetPlayerScoreData(UPlayerScoreData* InPlayerScoreData)
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("SetPlayerScoreData"));
		PlayerScoreData = Cast<UPlayerScoreData>(InPlayerScoreData->Clone(this));
	}

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerRank")
	int Rank;

	UFUNCTION(Server, Reliable)
	void ServerSetRank(int InRank);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerJob")
	EPlayerJob PlayerJob;

	void IncreaseScore(const TArray<EAnimal>& InAnimals);

	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

	virtual void RegisterPlayerWithSession(bool bWasFromInvite) override;

	/** Unregister a player with the online subsystem */
	virtual void UnregisterPlayerWithSession() override;
protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
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
	FTimerHandle PlayerNameTimerHandle;
};
