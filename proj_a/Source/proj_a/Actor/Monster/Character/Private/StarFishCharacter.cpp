#include "StarFishCharacter.h"

#include "Components/CapsuleComponent.h"


AStarFishCharacter::AStarFishCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STARFISH
	(TEXT("/Game/model/animals/starFish/star_fish.star_fish"));
	if (SK_STARFISH.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_STARFISH.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/starFishAnimation.starFishAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(32.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(32.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -28.f), FRotator(0.f, -90.f, 0.f));
}