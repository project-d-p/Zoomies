// LobbyPlatform.h

#pragma once

#include "CoreMinimal.h"
#include "DPCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "LobbyPlatform.generated.h"

UCLASS()
class PROJ_A_API ALobbyPlatform : public AActor
{
	GENERATED_BODY()

public:
	ALobbyPlatform();
	
	UFUNCTION(BlueprintCallable, Category = "Events")
	void SpawnCharacter(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Events")
	void Clear_Platform();
	
	UPROPERTY(VisibleAnywhere)
	APlayerController* PC = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	ADPCharacter* CurrentCharacter;
	

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CylinderComponent;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> CharacterClass;
};