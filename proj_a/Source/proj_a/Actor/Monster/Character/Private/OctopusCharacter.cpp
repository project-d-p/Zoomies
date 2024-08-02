#include "OctopusCharacter.h"

#include "Components/CapsuleComponent.h"


AOctopusCharacter::AOctopusCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_OCTOPUS
	(PathManager::GetMonsterPath(EAnimal::ANIMAL_OCTOPUS));
	if (SK_OCTOPUS.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_OCTOPUS.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(PathManager::GetMonsterAnimationPath(EAnimal::ANIMAL_OCTOPUS));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	DefaultCP.Radius = 100.f;
	DefaultCP.HalfHeight = 42.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 35.f;
	FaintCP.HalfHeight = 35.f;
	
	/** Set the model size and adjust position */
	FVector Location(0.f, 0.f, 40.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(0.f, 0.f, 90.f),
		FVector::ZeroVector,
		FVector::OneVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}