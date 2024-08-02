#include "CrabCharacter.h"

#include "Components/CapsuleComponent.h"

ACrabCharacter::ACrabCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CRAB
	(PathManager::GetMonsterPath(EAnimal::ANIMAL_CRAB));
	if (SK_CRAB.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CRAB.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(PathManager::GetMonsterAnimationPath(EAnimal::ANIMAL_CRAB));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	DefaultCP.Radius = 91.f;
	DefaultCP.HalfHeight = 91.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 91.f;
	FaintCP.HalfHeight = 91.f;

	/** Set the model size and adjust position */
	FVector Location(FVector::ZeroVector);
	FQuat Rotation = FQuat(FRotator(0.f, 90.f, 0.f));
	GetMesh()->SetRelativeLocationAndRotation(Location, Rotation);
	
	FTransform AdjMtx = FTransform(
		FQuat(FRotator(0.f, 0.f, 0.f)),
		Location);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FQuat(FRotator(90.f, 90.f, 0.f)),
		FVector::ZeroVector);
	CB_FaintStateMtx = AdjMtx.Inverse() * FaintStateMtx * AdjMtx;
}
