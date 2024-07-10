// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponGun.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "HitScan.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EntitySystem/MovieSceneEntitySystemTypes.h"
#include "GameFramework/Character.h"

ADPWeaponGun::ADPWeaponGun()
{
	damage = 15.f;
	hitScan = CreateDefaultSubobject<UHitScan>(TEXT("HitScan"));

	gunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	// RootComponent = gunMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GUN
	(TEXT("/Game/model/weapon/simpleGun.simpleGun"));
	if (GUN.Succeeded()) {
		gunMesh->SetStaticMesh(GUN.Object);
		gunMesh->SetupAttachment(RootComponent);
	}

	muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleComp"));
	muzzle->SetupAttachment(gunMesh);
	muzzle->SetRelativeLocation(FVector(-30.005441, 0.000000, 4.690275));
	muzzle->SetRelativeRotation(FRotator(0.000000, -180, 0.000000));

	// effects
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TRAIL
	(TEXT("/Game/effect/ns_gunTrail2.ns_gunTrail2"));
	if (TRAIL.Succeeded()) {
		trailEffect = TRAIL.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SMOKE
	(TEXT("/Game/effect/ns_gunSmokeSpark.ns_gunSmokeSpark"));
	if (SMOKE.Succeeded()) {
		smokeEffect = SMOKE.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PARTICLE
	(TEXT("/Game/effect/ns_animalHit.ns_animalHit"));
	if (PARTICLE.Succeeded()) {
		particleEffect = PARTICLE.Object;
	}
}

void ADPWeaponGun::BeginPlay()
{
	Super::BeginPlay();

	muzzle->RegisterComponent();
}

bool ADPWeaponGun::Attack(ADPPlayerController* controller, FHitResult& result, FRotator& info)
{
	const FVector start_aim_pos = controller->GetCharacter()->FindComponentByClass<UCameraComponent>()->GetComponentLocation();
	const FRotator aim_direction = controller->GetControlRotation();

	FVector impact_point;
	if (hitScan->HitDetect(Cast<ADPCharacter>(controller->GetCharacter()), start_aim_pos, aim_direction, 100000000.f, result, false))
	{
		impact_point = result.ImpactPoint;
	}
	else
	{
		impact_point = start_aim_pos + aim_direction.Vector() * 100000000.f;
	}

	FVector test_gun_mesh_location = gunMesh->GetComponentLocation();
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Start Location[gun Mesh]: %f, %f, %f"), test_gun_mesh_location.X, test_gun_mesh_location.Y, test_gun_mesh_location.Z);
	FVector test_weapon_location = this->GetActorLocation();
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Start Location[weapon]: %f, %f, %f"), test_weapon_location.X, test_weapon_location.Y, test_weapon_location.Z);
	
	FVector muzzle_direction = impact_point - muzzle->GetComponentLocation();
	FVector normalized_muzzle_direction = muzzle_direction.GetSafeNormal();
	info = normalized_muzzle_direction.Rotation();
	const FVector muzzle_location = muzzle->GetComponentLocation();
	
	return hitScan->HitDetect(Cast<ADPCharacter>(controller->GetCharacter()), muzzle_location, normalized_muzzle_direction.Rotation(), 100000000.f, result, false);
}

bool ADPWeaponGun::SimulateAttack(ADPCharacter* character, FHitResult& result, const Gunfire& gunfire)
{
	const FVector start = FVector(gunfire.position().x(), gunfire.position().y(), gunfire.position().z());
	const FRotator aim_direction = FRotator(gunfire.direction().x(), gunfire.direction().y(), gunfire.direction().z());
	return hitScan->HitDetect(character, start, aim_direction, 100000000.f, result);
}

FVector ADPWeaponGun::GetFireLocation()
{
	return muzzle->GetComponentLocation();
}

void ADPWeaponGun::SpawnEffects(const FHitResult& HitResult, const FRotator& rotation)
{
	bool bCond = true;
	// location은 총알이 맞은 위치.

	FVector ImpactPoint;
	if (HitResult.bBlockingHit)
	{
		ImpactPoint = HitResult.ImpactPoint;
	}
	else
	{
		ImpactPoint = muzzle->GetComponentLocation() + rotation.Vector() * 100000000.f;
		bCond = false;
	}

	FNetLogger::EditerLog(FColor::Cyan, TEXT("Impact Point[Weapon}: %f, %f, %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);

	UNiagaraComponent* trail = nullptr;

	if (trailEffect && muzzle) {
		trail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			trailEffect,
			muzzle->GetComponentLocation(),
			FRotator::ZeroRotator
		);
	}
	if (trail)
	{
		// 방향 설정: 월드 방향을 로컬 방향으로 변환
		FVector WorldDirection = (ImpactPoint - muzzle->GetComponentLocation()).GetSafeNormal();
		// FVector LocalDirection = muzzle->GetComponentTransform().InverseTransformVectorNoScale(WorldDirection);
		trail->SetVectorParameter(FName("Direction_FIRE"), WorldDirection * 100000.f); // 1000.f은 속도 조절을 위한 스칼라 값
	}
	
	if (smokeEffect && muzzle) {
		UNiagaraFunctionLibrary::SpawnSystemAttached(
		smokeEffect,
		muzzle,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true
		);
	}

	// Spawn the hit effect at the impact point
	if (bCond)
	{
		SpawnHitEffect(HitResult);
	}
}

void ADPWeaponGun::SpawnHitEffect(const FHitResult& HitResult)
{
	FVector ImpactPoint = HitResult.ImpactPoint;
	FVector ImpactNormal = HitResult.ImpactNormal;

	FRotator ParticleRotation = ImpactNormal.Rotation();
	if (particleEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			particleEffect,
			ImpactPoint,
			ParticleRotation
		);
	}
}
