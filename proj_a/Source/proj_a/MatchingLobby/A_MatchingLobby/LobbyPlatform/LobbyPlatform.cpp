// LobbyPlatform.cpp

#include "LobbyPlatform.h"

#include "DPCharacter.h"
#include "Components/BackgroundBlur.h"
#include "FNetLogger.h"
#include "Components/ArrowComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "UObject/ConstructorHelpers.h"

ALobbyPlatform::ALobbyPlatform()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	CylinderComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderComponent"));
	CylinderComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("'/Game/etc/Lobby/Shape_Cylinder.Shape_Cylinder'"));
	if (MeshAsset.Succeeded())
	{
		CylinderComponent->SetStaticMesh(MeshAsset.Object);
	}

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);

	PC = nullptr;
	CurrentCharacter = nullptr;
}

void ALobbyPlatform::SpawnCharacter(APlayerController* PlayerController)
{
	PC = PlayerController;

	if (CurrentCharacter && CurrentCharacter->IsValidLowLevel())
	{
		CurrentCharacter->Destroy();
		CurrentCharacter = nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentCharacter = GetWorld()->SpawnActor<ADPCharacter>(
		ADPCharacter::StaticClass(),
 		ArrowComponent->GetComponentTransform(),
 		SpawnParams);

	ADPCharacter* DPC = Cast<ADPCharacter>(CurrentCharacter);
	DPC->SetOwner(PC);
	if (PC->PlayerState)
	{
		DPC->SetPlayerState(PC->PlayerState);
	}	
	APC_MatchingLobby* PC_MatchingLobby = Cast<APC_MatchingLobby>(PC);
	if (PC_MatchingLobby && CurrentCharacter)
	{
		PC_MatchingLobby->Possess(CurrentCharacter);
		PC_MatchingLobby->SetCineCameraView();
		PC_MatchingLobby->bAutoManageActiveCameraTarget = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ALobbyPlatform::SpawnCharacter: Failed to cast PC_MatchingLobby"));
	}
 }

void ALobbyPlatform::Clear_Platform()
{
	PC = nullptr;

	if (CurrentCharacter && CurrentCharacter->IsValidLowLevel())
	{
		TArray<AActor*> AttachedActors;
		CurrentCharacter->GetAttachedActors(AttachedActors);

		for (AActor* Actor : AttachedActors)
		{
			if (Actor)
			{
				Actor->Destroy();
			}
		}
		CurrentCharacter->Destroy();
		CurrentCharacter = nullptr;
	}
}