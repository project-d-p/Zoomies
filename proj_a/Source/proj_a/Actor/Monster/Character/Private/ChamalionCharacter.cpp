#include "ChamalionCharacter.h"

#include "Components/CapsuleComponent.h"

AChamalionCharacter::AChamalionCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK
	(PathManager::GetMonsterPath(EMonster::CHAMALEON));
	if (SK.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/chamalionAnimation.chamalionAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	DefaultCP.Radius = 88.f;
	DefaultCP.HalfHeight = 34.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 42.f;
	FaintCP.HalfHeight = 42.f;

	/** Set the model size and adjust position */
	FVector Location(0.f, 0.f, 0.f);
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
