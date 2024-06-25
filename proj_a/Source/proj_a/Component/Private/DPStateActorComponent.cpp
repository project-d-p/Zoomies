// Fill out your copyright notice in the Description page of Project Settings.

#include "DPStateActorComponent.h"

// Sets default values for this component's properties
UDPStateActorComponent::UDPStateActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UDPStateActorComponent::BeginPlay()
{
	Super::BeginPlay();

	characterState = "IDLE";
	equipmentState = "GUN";
}


// Called every frame
void UDPStateActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UDPStateActorComponent::ChangeCharacterState(int value)
{
	currentCharacterState += value;
	if (-1 == currentCharacterState)
		currentCharacterState = 7;
	if (8 == currentCharacterState)
		currentCharacterState = 0;
	
	if (0 == currentCharacterState) {
		characterState = "IDLE";
	}
	if (1 == currentCharacterState) {
		characterState = "WALK";
	}
	if (2 == currentCharacterState) {
		characterState = "RUN";
	}
	if (3 == currentCharacterState) {
		characterState = "SPRINT";
	}	
	if (4 == currentCharacterState) {
		characterState = "JUMP";
	}
	if (5 == currentCharacterState) {	// 행동
		characterState = "INTERACTION";
	}
	if (6 == currentCharacterState) {	// 채팅 상점 등
		characterState = "OUT_FOCUS";
	}
	if (7 == currentCharacterState) {
		characterState = "DIE";
	}

	UE_LOG(LogTemp, Warning, TEXT("ChangeCharacterState : %s"), *characterState);
}

void UDPStateActorComponent::ChangeEquipmentState(int value)
{
	currentEquipmentState += value;
	if (-1 == currentEquipmentState)
		currentEquipmentState = 3;
	if (4 == currentEquipmentState)
		currentEquipmentState = 0;

	if (0 == currentEquipmentState) {
		equipmentState = "NONE";
	}
	if (1 == currentEquipmentState) {
		equipmentState = "GUN";
	}
	if (2 == currentEquipmentState) {
		equipmentState = "WALL";
	}
	if (3 == currentEquipmentState) {
		equipmentState = "TURRET";
	}

	UE_LOG(LogTemp, Warning, TEXT("ChangeEquipmentState : %s"), *equipmentState);
}
