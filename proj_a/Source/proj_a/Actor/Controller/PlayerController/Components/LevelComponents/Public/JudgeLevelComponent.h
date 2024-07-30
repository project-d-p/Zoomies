#pragma once

#include "CoreMinimal.h"
#include "BaseLevelComponent.h"
#include "JudgeLevelUI.h"
#include "JudgeLevelComponent.generated.h"

UCLASS()
class PROJ_A_API UJudgeLevelComp : public UBaseLevelComponent
{
	GENERATED_BODY()
public:
	UJudgeLevelComp();
	void CreateJudgeLevelUI();
	UJudgeLevelUI* GetJudgeLevelUI() const { return JudgeLevelUI; }

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	UJudgeLevelUI* JudgeLevelUI;
};
