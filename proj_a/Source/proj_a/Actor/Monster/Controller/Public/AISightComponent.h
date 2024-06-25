// AISightComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AISightComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDetectedDelegate, AActor*, DetectedEnemy);

UCLASS()
class PROJ_A_API UAISightComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAISightComponent();
    void ConfigureSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees);

    UPROPERTY()
    FOnEnemyDetectedDelegate OnEnemyDetected;
private:
    void ActivateSensing();

    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
    
    UPROPERTY()
    UAIPerceptionComponent* PerceptionComponent;
    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;
    
protected:
    virtual void BeginPlay() override;
};