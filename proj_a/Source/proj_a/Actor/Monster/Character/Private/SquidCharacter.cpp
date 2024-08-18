#include "SquidCharacter.h"

#include "Components/CapsuleComponent.h"


ASquidCharacter::ASquidCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SQUID
	(PathManager::GetMonsterPath(EAnimal::ANIMAL_SQUID));
	if (SK_SQUID.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_SQUID.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		PathManager::GetMonsterAnimationPath(EAnimal::ANIMAL_SQUID));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	DefaultCP.Radius = 64.f;
	DefaultCP.HalfHeight = 174.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 64.f;
	FaintCP.HalfHeight = 64.f;
	
	/** Set the model size and adjust position */
	FVector Scale(FVector::OneVector);
	FVector Location(0.f, 0.f, 0.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location, Scale);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(0.f, 0.f, 90.f),
		FVector::ZeroVector,
		FVector::OneVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}
