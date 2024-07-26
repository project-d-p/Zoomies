// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeapon.h"

#include "DPPlayerController.h"
//#include "DPCharacter.h"

// Sets default values
ADPWeapon::ADPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SoundComponent = CreateDefaultSubobject<USoundComponent>(TEXT("SoundComponent"));
	damage = 20.f;
}

// Called when the game starts or when spawned
void ADPWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADPWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ADPWeapon::Attack(ADPPlayerController* controller, FHitResult& result, FRotator& info)
{
	UE_LOG(LogTemp, Warning, TEXT("attack weapon"));
	return false;
}

bool ADPWeapon::SimulateAttack(ADPCharacter* character, FHitResult& result, const Gunfire& gunfire)
{
	return false;
}

FVector ADPWeapon::GetFireLocation()
{
	return FVector();
}

void ADPWeapon::SpawnEffects(const FHitResult& location, const FRotator& rotation)
{
}
