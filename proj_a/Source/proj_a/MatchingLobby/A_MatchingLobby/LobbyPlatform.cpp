// LobbyPlatform.cpp

#include "LobbyPlatform.h"

#include "DPCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/CHAR_MatchingLobby/CHAR_MatchingLobby.h"
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

	CurrentCharacter = GetWorld()->SpawnActor<ACHAR_MatchingLobby>(
		ACHAR_MatchingLobby::StaticClass(),
 		ArrowComponent->GetComponentTransform(),
 		SpawnParams);
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