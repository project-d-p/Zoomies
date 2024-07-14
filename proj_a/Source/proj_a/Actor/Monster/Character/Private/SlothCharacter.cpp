#include "SlothCharacter.h"

#include "Components/CapsuleComponent.h"


ASlothCharacter::ASlothCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SLOTH
	(PathManager::GetPath(EMonster::SLOTH));
	if (SK_SLOTH.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_SLOTH.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/slothAnimation.slothAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}
	
	/** Set the Capsule size */
	DefaultCP.Radius = 120.f;
	DefaultCP.HalfHeight = 78.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 75.f;
	FaintCP.HalfHeight = 75.f;
	
	/** Set the model size and adjust position */
	FVector Location(0.f, 0.f, 10.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(0.f, 0.f, 90.f),
		FVector::ZeroVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}