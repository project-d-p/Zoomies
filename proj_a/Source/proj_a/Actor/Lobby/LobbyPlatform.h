// LobbyPlatform.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "LobbyPlatform.generated.h"

UCLASS()
class PROJ_A_API ALobbyPlatform : public AActor
{
	GENERATED_BODY()

public:
	ALobbyPlatform();

public:

	UFUNCTION(BlueprintCallable, Category = "Events")
	void SpawnCharacter(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Events")
	void Clear_Platform();
	
	UPROPERTY(VisibleAnywhere)
	APlayerController* PC = nullptr;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CylinderComponent;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> CharacterClass;


	UPROPERTY(VisibleAnywhere)
	AActor* CurrentCharacter;
};