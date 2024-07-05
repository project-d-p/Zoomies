#include "ElephantCharacter.h"

#include "Components/CapsuleComponent.h"


AElephantCharacter::AElephantCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ELEPHANT
	(TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant"));
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
	
	/** Set the model size */
	GetMesh()->SetRelativeScale3D(FVector(1.00f, 1.00f, 1.00f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -20.f), FRotator(0.f, -90.f, 0.f));
}
