// Fill out your copyright notice in the Description page of Project Settings.


#include "DPHpActorComponent.h"

// Sets default values for this component's properties
UDPHpActorComponent::UDPHpActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	maxHP = 100.f;
}


// Called when the game starts
void UDPHpActorComponent::BeginPlay()
{
	Super::BeginPlay();

	Hp = maxHP;
}


// Called every frame
void UDPHpActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

