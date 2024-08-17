#include "BlockingBoxVolume.h"

ABlockingBoxVolume::ABlockingBoxVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	BlockingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox"));
	BlockingBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BlockingBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BlockingBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BlockingBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
	RootComponent = BlockingBox;

	BlockingBox->SetRelativeLocation(FVector(-620.000000, -30.000000, 140.000000));
	BlockingBox->SetRelativeScale3D(FVector(0.250000,13.250000,7.500000));
}

void ABlockingBoxVolume::DeactiveBlockingVolume()
{
	FTimerHandle DeactiveTimer;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]() {
		BlockingBox->DestroyComponent();
	});
	GetWorld()->GetTimerManager().SetTimer(DeactiveTimer, TimerDelegate, 4.0f, false);
}

