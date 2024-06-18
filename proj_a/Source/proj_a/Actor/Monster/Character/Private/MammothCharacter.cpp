#include "MammothCharacter.h"

#include "Components/CapsuleComponent.h"

AMammothCharacter::AMammothCharacter()
{
	/** Add custom Hp later */
	
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH(TEXT("/Game/model/animals/mammoth/sm_mammoth.sm_mammoth"));
	if (SK_MAMMOTH.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_MAMMOTH.Object);
	}
	/** Set the model size */
	GetMesh()->SetRelativeScale3D(FVector(3.00f, 3.00f, 3.00f));
}
