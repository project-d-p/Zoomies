#include "ElephantCharacter.h"

#include "Components/CapsuleComponent.h"


AElephantCharacter::AElephantCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ELEPHANT
	(PathManager::GetMonsterPath(EMonster::ELEPHANT));
	if (SK_ELEPHANT.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_ELEPHANT.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		TEXT("/Game/animation/animals/elephantAnimation.elephantAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(65.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(65.f);
	
	/** Set the model size and adjust position */
	FVector Scale(FVector::OneVector);
	FVector Location(0.f, 0.f, 0.f);
	FRotator Rotation(0.f, 90.f, 0.f);
	FTransform Transform(Rotation, Location, Scale);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(90.f, 0.f, 0.f),
		FVector::ZeroVector,
		FVector::OneVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}
