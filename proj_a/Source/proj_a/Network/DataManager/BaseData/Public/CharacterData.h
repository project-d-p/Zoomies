#pragma once

#include "CoreMinimal.h"
#include "BaseData.h"
#include "EnumTypes.h"
#include "CharacterData.generated.h"

UCLASS()
class UCharacterData : public UBaseData
{
	GENERATED_BODY()
public:
	virtual void InitializeData_Implementation() override;
	virtual bool ValidateData_Implementation() override;
	virtual void ResetData_Implementation() override;
	virtual UBaseData* Clone_Implementation(UObject* Outer) override;
	virtual FName GetDataType() const override;
	void SetActorName(const FString& NewName);
	void SetActorLocation(const FVector& NewLocation);
	void SetActorRotation(const FRotator& NewRotation);
	void AddCapturedAnimal(EAnimal NewAnimal);

	TArray<EAnimal> GetCapturedAnimals() const;
	FString GetActorName() const;
	FVector GetActorLocation() const;
	FRotator GetActorRotation() const;
private:
	FString ActorName;
	FVector ActorLocation;
	FRotator ActorRotation;
	TArray<EAnimal> CapturedAnimals;
};
