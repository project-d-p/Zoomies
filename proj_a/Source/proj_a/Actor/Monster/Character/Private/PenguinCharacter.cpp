#include "PenguinCharacter.h"

#include "Components/CapsuleComponent.h"

APenguinCharacter::APenguinCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX(
		TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming"));
	if (SK_FOX.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_FOX.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		TEXT("/Game/animation/animals/penguinAnimation.penguinAnimation_C"));
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
