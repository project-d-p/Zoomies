#pragma once

#include "CoreMinimal.h"
// #include "PlayerScoreComp.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "DPPlayerState.generated.h"

UCLASS()
class PROJ_A_API ADPPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADPPlayerState();
	
	UFUNCTION()
	UPlayerScoreComp* GetPlayerScoreComp() const;

	UFUNCTION()
	EPlayerJob GetPlayerJob() const;

	UPROPERTY(ReplicatedUsing = OnRep_Rank, BlueprintReadWrite, Category = "PlayerRank")
	int Rank;

	UFUNCTION()
	void OnRep_Rank();

	UFUNCTION(Server, Reliable)
	void ServerSetRank(int InRank);
	
private:
	UPROPERTY()
	UPlayerScoreComp* PlayerScoreComp = nullptr;

	UPROPERTY(Replicated)
	EPlayerJob PlayerJob = EPlayerJob::JOB_ARCHAEOLOGIST;

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
