#include "LionCharacter.h"

#include "Components/CapsuleComponent.h"


ALionCharacter::ALionCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LION
	(TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion"));
	if (SK_LION.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_LION.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/lionAnimation.lionAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(65.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(65.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -20.f), FRotator(0.f, -90.f, 0.f));
}