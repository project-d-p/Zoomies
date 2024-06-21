#include "FoxCharacter.h"

AFoxCharacter::AFoxCharacter()
{
	/** Loading models */
    	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX(TEXT("/Game/model/animals/fox_1/fennec_fox.fennec_fox"));
    	if (SK_FOX.Succeeded()) {
    		GetMesh()->SetSkeletalMesh(SK_FOX.Object);
    	}
    /** Set the model size */
    GetMesh()->SetRelativeScale3D(FVector(1.00f, 1.00f, 1.00f));
}
