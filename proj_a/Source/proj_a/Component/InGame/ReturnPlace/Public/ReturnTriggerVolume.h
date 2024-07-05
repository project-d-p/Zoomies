#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "ReturnTriggerVolume.generated.h"

UCLASS()
class AReturnTriggerVolume : public AActor
{
	GENERATED_BODY()
public:
	AReturnTriggerVolume();

protected:
	virtual void BeginPlay() override;
	
private:
	// Box component to represent the volume
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerSphere;

	// Function to handle the overlap beginning
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function to handle the overlap ending
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
