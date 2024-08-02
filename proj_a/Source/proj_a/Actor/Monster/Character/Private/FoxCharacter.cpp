#include "FoxCharacter.h"

#include "Components/CapsuleComponent.h"


AFoxCharacter::AFoxCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX
	(PathManager::GetMonsterPath(EAnimal::ANIMAL_FOX));
    if (SK_FOX.Succeeded()) {
    	GetMesh()->SetSkeletalMesh(SK_FOX.Object);
    }

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(PathManager::GetMonsterAnimationPath(EAnimal::ANIMAL_FOX));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	DefaultCP.Radius = 72.f;
	DefaultCP.HalfHeight = 150.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 52.f;
	FaintCP.HalfHeight = 52.f;
	
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
