﻿#include "DynamicTexturedCharacter.h"

#include "FNetLogger.h"
#include "TextureTransferManager.h"

ADynamicTexturedCharacter::ADynamicTexturedCharacter()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CHARACTER
	(TEXT("/Game/model/steve/StickManForMixamo.StickManForMixamo"));
	if (SK_CHARACTER.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CHARACTER.Object);
	}
	bNetLoadOnClient = true;
	bReplicates = true;
	NetworkedDynamicTexture = CreateDefaultSubobject<UNetworkedDynamicTextureComponent>(TEXT("UNetworkedDynamicTextureComponent"));
}

void ADynamicTexturedCharacter::Client_SetOwner_Implementation(APlayerController* NewOwner)
{
	if (NewOwner)
	{
		SetOwner(NewOwner);
	}
}

void ADynamicTexturedCharacter::BeginPlay()
{
	Super::BeginPlay();

	// TryInItializeDynamicTexture();
	dynamicMaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this, TEXT("DynamicMaterial"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitializeDynamicTexture, this, &ADynamicTexturedCharacter::TryInItializeDynamicTexture, 1.5f, true);
}

void ADynamicTexturedCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// TryInItializeDynamicTexture();
}

void ADynamicTexturedCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	// TryInItializeDynamicTexture();
}

void ADynamicTexturedCharacter::OnRep_Owner()
{
	Super::OnRep_Owner();

	// TryInItializeDynamicTexture();
}

void ADynamicTexturedCharacter::TryInItializeDynamicTexture()
{
	if (bInitialized || !dynamicMaterialInstance || !NetworkedDynamicTexture || bPlayerAssigned)
	{
		return;
	}
	FNetworkedDynamicTextureComponentInitializer Initializer;
	Initializer.DynamicMaterialInstance = dynamicMaterialInstance;
	Initializer.SkeletalMeshComponent = GetMesh();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		Initializer.PlayerState = GetPlayerState<APlayerState>();
		Initializer.TextureTransferManager = PC->FindComponentByClass<UTextureTransferManager>();
	}
	if (NetworkedDynamicTexture->InitializeTexture(Initializer))
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("TryInItializeDynamicTexture Success"));
		NetworkedDynamicTexture->LoadTexture();
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("TryInItializeDynamicTexture failed"));
	}
}
