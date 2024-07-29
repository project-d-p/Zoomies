#include "SaberToothTigerCharacter.h"

#include "Components/CapsuleComponent.h"


ASaberToothTigerCharacter::ASaberToothTigerCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SABERTOOTHTIGER
	(PathManager::GetMonsterPath(EAnimal::ANIMAL_SABER_TOOTH_TIGER));
	if (SK_SABERTOOTHTIGER.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_SABERTOOTHTIGER.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		PathManager::GetMonsterAnimationPath(EAnimal::ANIMAL_SABER_TOOTH_TIGER));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	DefaultCP.Radius = 70.f;
	DefaultCP.HalfHeight = 150.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 70.f;
	FaintCP.HalfHeight = 70.f;
	
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
