#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION()
	EPlayerJob GetPlayerJob() const;

	void SetPlayerRandomJob();
	void SetFinalScoreData(const FFinalScoreData& InFinalScoreData) { FinalScoreData = InFinalScoreData; }
	const FFinalScoreData& GetFinalScoreData() const { return FinalScoreData; }

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerRank")
	int Rank;

	UFUNCTION(Server, Reliable)
	void ServerSetRank(int InRank);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PlayerJob")
	EPlayerJob PlayerJob;
protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
private:
	UPROPERTY()
	UPlayerScoreComp* PlayerScoreComp = nullptr;
	
	FFinalScoreData FinalScoreData;
	UPlayerScoreData* PlayerScoreData = nullptr;

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
