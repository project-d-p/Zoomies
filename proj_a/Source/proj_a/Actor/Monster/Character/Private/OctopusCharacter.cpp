#include "OctopusCharacter.h"

#include "Components/CapsuleComponent.h"


AOctopusCharacter::AOctopusCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_OCTOPUS
	(TEXT("/Game/model/animals/octopus/Octopus1.Octopus1"));
	if (SK_OCTOPUS.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_OCTOPUS.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/octopusAnimation.octopusAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(110.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(60.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(30.f, 0.f, -42.f), FRotator(0.f, 0.f, 0.f));
}