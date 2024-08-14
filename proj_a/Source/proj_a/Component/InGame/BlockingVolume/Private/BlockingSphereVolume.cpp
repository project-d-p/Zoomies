#include "BlockingSphereVolume.h"

ABlockingSphereVolume::ABlockingSphereVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// ���� ��ü: Overlap ó��
	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InnerVolume"));
	InnerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InnerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	InnerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InnerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InnerSphere->SetSphereRadius(750.0f);  // ���� ��ü�� �ݰ� ����

	// �ܺ� ��ü: ��迡�� Block ó��
	BlockingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BlockingVolume"));
	BlockingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BlockingSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BlockingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BlockingSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	BlockingSphere->SetSphereRadius(800.0f);  // �ܺ� ��ü�� �ݰ� ����

	// ���� ��ü�� ��Ʈ ������Ʈ�� ����
	RootComponent = InnerSphere;

	// �ܺ� ��ü�� ���� ��ü�� ����
	BlockingSphere->SetupAttachment(InnerSphere);

	InnerSphere->SetRelativeLocation(FVector(0, 0, 0));
	BlockingSphere->SetRelativeLocation(FVector(0, 0, 0));
}

void ABlockingSphereVolume::DeactiveBlockingVolume()
{
	FTimerHandle DeactiveTimer;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]() {
		BlockingSphere->DestroyComponent();
		InnerSphere->DestroyComponent();
	});
	GetWorld()->GetTimerManager().SetTimer(DeactiveTimer, TimerDelegate, 4.0f, false);
}

