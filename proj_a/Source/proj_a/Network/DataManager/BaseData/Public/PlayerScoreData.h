#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "ScoreTypes.h"
#include "PlayerScoreData.generated.h"

UCLASS()
class UPlayerScoreData : public UBaseData
{
	GENERATED_BODY()
public:
	virtual void InitializeData_Implementation() override;
	virtual bool ValidateData_Implementation() override;
	virtual void ResetData_Implementation() override;
	// virtual void SaveData_Implementation() override;
	// virtual void LoadData_Implementation() override;
	virtual UBaseData* Clone_Implementation(UObject* Outer) override;
	virtual FName GetDataType() const override;
	void IncreaseScore(const EPlayerJob& PlayerJob, const TArray<EAnimal>& Animals);
	void TestBroadcast();
	void SetPlayerName(const FString& String);
	FString GetPlayerName() const { return PlayerName; }
	FFinalScoreData GetScore() const { return Score; }
	void SetScore(const FFinalScoreData& InScore) { Score = InScore; }
	void SetIsDetected(bool bIsDetected) { Score.bIsDetected = bIsDetected; }

private:
	FString PlayerName;
	FFinalScoreData Score;
};
