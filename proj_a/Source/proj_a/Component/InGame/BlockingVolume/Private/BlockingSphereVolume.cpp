#include "BlockingSphereVolume.h"

ABlockingSphereVolume::ABlockingSphereVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// 내부 구체: Overlap 처리
	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InnerVolume"));
	InnerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InnerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	InnerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InnerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InnerSphere->SetSphereRadius(750.0f);  // 내부 구체의 반경 설정

	// 외부 구체: 경계에서 Block 처리
	BlockingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BlockingVolume"));
	BlockingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BlockingSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BlockingSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BlockingSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	BlockingSphere->SetSphereRadius(800.0f);  // 외부 구체의 반경 설정

	// 내부 구체를 루트 컴포넌트로 설정
	RootComponent = InnerSphere;

	// 외부 구체는 내부 구체에 부착
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

