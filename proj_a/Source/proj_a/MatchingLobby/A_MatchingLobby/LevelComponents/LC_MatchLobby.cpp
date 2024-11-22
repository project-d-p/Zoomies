#include "LC_MatchLobby.h"

#include "BasicInputComponent.h"
#include "MainInputComponent.h"
#include "../InputComponents/IC_MatchLobby.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"

ULC_MatchLobby::ULC_MatchLobby()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputComponent = CreateDefaultSubobject<UIC_MatchLobby>(TEXT("IC_MatchLobby"));
	InputComponent->Set_LC(this);
}

void ULC_MatchLobby::SetFixedCameraView()
{
	APC_MatchingLobby* PC = Cast<APC_MatchingLobby>(GetOwner());
	check(PC)
	ADPCharacter* Char = Cast<ADPCharacter>(PC->GetCharacter());
	check(Char)
	Char->RemoveSpringArm();
}

void ULC_MatchLobby::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void ULC_MatchLobby::Deactivate()
{
	Super::Deactivate();
	InputComponent->Deactivate();
}

void ULC_MatchLobby::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULC_MatchLobby::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Deactivate();
	DestroyComponent();
}

APC_MatchingLobby* ULC_MatchLobby::GetPlayerController() const
{
	return PC_MatchLobby;
}

APawn* ULC_MatchLobby::GetPlayerCharacter()
{
	if (PC_MatchLobby != nullptr)
	{
		CHAR_MatchLobby = PC_MatchLobby->GetPawn();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LC_MatchLobby::PlayerController is nullptr."));
	}
	return CHAR_MatchLobby;
}

void ULC_MatchLobby::Set_PC(APC_MatchingLobby* AdpPlayerController)
{
	PC_MatchLobby = AdpPlayerController;
	if (!AdpPlayerController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("LC_MatchLobby::PlayerController is not local"));
	}
	InputComponent->Set_PC(AdpPlayerController);
}

void ULC_MatchLobby::Set_CHAR(APawn* InCharacter)
{
	CHAR_MatchLobby = InCharacter;
}

UIC_MatchLobby* ULC_MatchLobby::GetInputComponent() const
{
	return InputComponent;
}

USoundComponent* ULC_MatchLobby::GetSoundComponent() const
{
	return SoundComponent;
}
