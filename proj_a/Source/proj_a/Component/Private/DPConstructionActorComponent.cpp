// Fill out your copyright notice in the Description page of Project Settings.


#include "DPConstructionActorComponent.h"
#include "DPCharacter.h"

// Sets default values for this component's properties
UDPConstructionActorComponent::UDPConstructionActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	maxWall = 10;
}


// Called when the game starts
void UDPConstructionActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UDPConstructionActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDPConstructionActorComponent::MakeWall(FVector pos, FRotator rotation)
{
	ADPCharacter* owner = Cast<ADPCharacter>(GetOwner());

	if (owner)
		owner->SetActorLocationAndRotation(pos, rotation);
}

