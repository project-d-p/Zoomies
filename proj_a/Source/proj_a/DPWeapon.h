// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DPWeapon.generated.h"

UCLASS()
class PROJ_A_API ADPWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADPWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int maxBullet{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int bulletCount{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float damage{};
};
