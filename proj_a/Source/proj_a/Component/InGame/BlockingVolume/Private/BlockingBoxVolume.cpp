#include "BlockingBoxVolume.h"

ABlockingBoxVolume::ABlockingBoxVolume()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;

	// UStaticMeshComponent를 생성하고 기본 박스 메쉬를 할당합니다.
	BlockingBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockingBox"));
    
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (BoxMesh.Succeeded())
	{
		BlockingBox->SetStaticMesh(BoxMesh.Object);
	}
    
	// 콜리전 설정 (필요에 따라 적용)
	BlockingBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BlockingBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BlockingBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BlockingBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
	RootComponent = BlockingBox;

	//BlockingBox->SetRelativeLocation(FVector(-620.000000, -30.000000, 140.000000));
	BlockingBox->SetRelativeScale3D(FVector(0.001f, 5.25f, 1.25f));

	// 머티리얼 적용
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlockingWallMaterial(TEXT("/Game/material/m_blockingWall.m_blockingWall"));
	if (BlockingWallMaterial.Succeeded())
	{
		BlockingBox->SetMaterial(0, BlockingWallMaterial.Object);
	}
}

void ABlockingBoxVolume::DeactiveBlockingVolume(bool& bWallDisappear)
{
	FTimerHandle DeactiveTimer;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, &bWallDisappear]() {
		if (HasAuthority())
		{
			MulticastDestroyComponent();
			bWallDisappear = true;
		}
	});
	GetWorld()->GetTimerManager().SetTimer(DeactiveTimer, TimerDelegate, 10.0f, false);
}

void ABlockingBoxVolume::MulticastDestroyComponent_Implementation()
{
	BlockingBox->DestroyComponent();
}


