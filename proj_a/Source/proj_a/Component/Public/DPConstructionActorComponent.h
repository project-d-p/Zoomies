// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DPConstructionActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_A_API UDPConstructionActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDPConstructionActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int maxWall{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	int wallCount{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	bool placeWall{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
	bool placeturret{ false };

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void MakeWall(FVector pos, FRotator rotation);
};
