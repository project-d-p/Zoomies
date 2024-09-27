#include "CharacterData.h"

void UCharacterData::InitializeData_Implementation()
{
	Super::InitializeData_Implementation();
	ActorName = TEXT("");
	ActorLocation = FVector::ZeroVector;
	ActorRotation = FRotator::ZeroRotator;
}

bool UCharacterData::ValidateData_Implementation()
{
	bool bIsValid = Super::ValidateData_Implementation();
	bIsValid &= !ActorName.IsEmpty();
	return bIsValid;
}

void UCharacterData::ResetData_Implementation()
{
	Super::ResetData_Implementation();
	ActorName = TEXT("");
	ActorLocation = FVector::ZeroVector;
	ActorRotation = FRotator::ZeroRotator;
}

UBaseData* UCharacterData::Clone_Implementation(UObject* Outer)
{
	UCharacterData* ClonedObject = Cast<UCharacterData>(Super::Clone_Implementation(Outer));
	if (ClonedObject)
	{
		ClonedObject->ActorName = ActorName;
		ClonedObject->ActorLocation = ActorLocation;
		ClonedObject->ActorRotation = ActorRotation;
	}
	return ClonedObject;
}

FName UCharacterData::GetDataType() const
{
	return Super::GetDataType();
}

void UCharacterData::SetActorName(const FString& NewName)
{
	ActorName = NewName;
}

void UCharacterData::SetActorLocation(const FVector& NewLocation)
{
	ActorLocation = NewLocation;
}

void UCharacterData::SetActorRotation(const FRotator& NewRotation)
{
	ActorRotation = NewRotation;
}

FString UCharacterData::GetActorName() const
{
	return ActorName;
}

FVector UCharacterData::GetActorLocation() const
{
	return ActorLocation;
}

FRotator UCharacterData::GetActorRotation() const
{
	return ActorRotation;
}