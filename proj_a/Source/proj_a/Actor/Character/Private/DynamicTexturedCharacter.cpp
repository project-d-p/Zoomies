#include "DynamicTexturedCharacter.h"

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
	NetworkedDynamicTexture = CreateDefaultSubobject<UNetworkedDynamicTextureComponent>(TEXT("UNetworkedDynamicTextureComponent"));
}

void ADynamicTexturedCharacter::BeginPlay()
{
	Super::BeginPlay();

	// TryInItializeDynamicTexture();
	dynamicMaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this, TEXT("DynamicMaterial"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitializeDynamicTexture, this, &ADynamicTexturedCharacter::TryInItializeDynamicTexture, 0.5f, true);
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
	if (bInitialized || !dynamicMaterialInstance || !NetworkedDynamicTexture)
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
		NetworkedDynamicTexture->LoadTexture();
	}
}
