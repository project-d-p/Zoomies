#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "JudgeData.generated.h"

UCLASS()
class UJudgeData : public UBaseData
{
	GENERATED_BODY()
public:
	virtual void InitializeData_Implementation() override;
	virtual bool ValidateData_Implementation() override;
	virtual void ResetData_Implementation() override;
	virtual UBaseData* Clone_Implementation(UObject* Outer) override;
	virtual FName GetDataType() const override;
	void AddJudgedPlayerName(const FString& NewName);
	bool IsJudgedPlayer(const FString& Name) const;
	void SetJudgedPlayerNames(const TArray<FString>& Names) { JudgedPlayerNames = Names; }
	const TArray<FString>& GetJudgedPlayerNames() const { return JudgedPlayerNames; }
	
private:
	TArray<FString> JudgedPlayerNames;
};
