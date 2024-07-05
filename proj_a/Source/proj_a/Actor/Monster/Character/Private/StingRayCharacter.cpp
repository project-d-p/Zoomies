#include "StingRayCharacter.h"

#include "Components/CapsuleComponent.h"


AStingRayCharacter::AStingRayCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STINGRAY
	(TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray"));
	if (SK_STINGRAY.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_STINGRAY.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		TEXT("/Game/animation/animals/stingRayAnimation.stingRayAnimation_C"));
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
