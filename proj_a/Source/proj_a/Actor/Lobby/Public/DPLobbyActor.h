// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DPLobbyActor.generated.h"

USTRUCT()
struct FAnimalData
{
    GENERATED_BODY()

    UPROPERTY()
    USkeletalMeshComponent* meshComponent;

    UPROPERTY()
    FVector offset;

    FAnimalData() : meshComponent(nullptr), offset(FVector::ZeroVector) {}
};

UCLASS()
class PROJ_A_API ADPLobbyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADPLobbyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* foxComponent;
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* giraffeComponent;
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* racoonComponent;   
    //UPROPERTY(VisibleAnywhere)
    //USkeletalMeshComponent* rabbitComponent;    
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* crabComponent;    
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* turtleComponent;    
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* lionComponent;

    UPROPERTY(VisibleAnywhere)
    class UArrowComponent* arrowComponent;

    //UPROPERTY(VisibleAnywhere)
    //UAudioComponent* foxAudioComponent;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* foxAnim;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* giraffeAnim;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* racoonAnim;
    //UPROPERTY(EditAnywhere, Category = "Animation")
    //UAnimSequence* rabbitAnim;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* crabAnim;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* turtleAnim;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* lionAnim;

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* boxComponent;

    FVector foxForwardLocation;
    FVector initialLocation;
    void CheckReach();

	UPROPERTY()
    TArray<FAnimalData> animalComponents;
};
