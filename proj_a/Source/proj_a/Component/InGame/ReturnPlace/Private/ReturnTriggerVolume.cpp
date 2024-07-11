#include "ReturnTriggerVolume.h"

#include "DPCharacter.h"
#include "FNetLogger.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"

AReturnTriggerVolume::AReturnTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and configure the box component
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = TriggerSphere;

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AReturnTriggerVolume::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AReturnTriggerVolume::OnOverlapEnd);
	
	TriggerSphere->SetRelativeLocation(FVector(-1.185065, 1.450112, -4.687501));
	TriggerSphere->SetSphereRadius(750.0f);
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> EFFECTS_RETURN
	(TEXT("/Game/effect/ns_teleport.ns_teleport"));
	if (EFFECTS_RETURN.Succeeded()) {
		EffectsReturn = EFFECTS_RETURN.Object;
	}

	InitializeMonsterMeshes();
}

void AReturnTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
}

void AReturnTriggerVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADPCharacter* Character = Cast<ADPCharacter>(OtherActor);
	if (!Character)
	{
		return ;
	}
	Character->SetAtReturnPlace(true);
}

void AReturnTriggerVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADPCharacter* Character = Cast<ADPCharacter>(OtherActor);
	if (!Character)
	{
		return ;
	}
	Character->SetAtReturnPlace(false);
}

void AReturnTriggerVolume::InitializeMonsterMeshes()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CRAB
	(TEXT("/Game/model/animals/crab/crab.crab"));
	if (SK_CRAB.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_CRAB] = SK_CRAB.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_DOLPHIN
	(TEXT("/Game/model/animals/dolphin/dolphin1.dolphin1"));
	if (SK_DOLPHIN.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_DOLPHIN] = SK_DOLPHIN.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_EEL
	(TEXT("/Game/model/animals/eel/eel.eel"));
	if (SK_EEL.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_EEL] = SK_EEL.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ELEPHANT
    (TEXT("/Game/model/animals/elephant/low_poly_elephant.low_poly_elephant"));
    if (SK_ELEPHANT.Succeeded()) {
    	monsterMeshMap[EAnimal::ANIMAL_ELEPHANT] = SK_ELEPHANT.Object;
    }

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOX
	(TEXT("/Game/model/animals/fox_1/fennec_fox.fennec_fox"));
	if (SK_FOX.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_FOX] = SK_FOX.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFE
	(TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly"));
	if (SK_GIRAFFE.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_GIRAFFE] = SK_GIRAFFE.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LION
	(TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion"));
	if (SK_LION.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_LION] = SK_LION.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LOBSTER
	(TEXT("/Game/model/animals/lobstar/Object_7.Object_7"));
	if (SK_LOBSTER.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_LOBSTER] = SK_LOBSTER.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH
	(TEXT("/Game/model/animals/mammoth/sm_mammoth.sm_mammoth"));
	if (SK_MAMMOTH.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_MAMMOTH] = SK_MAMMOTH.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_OCTOPUS
	(TEXT("/Game/model/animals/octopus/Octopus1.Octopus1"));
	if (SK_OCTOPUS.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_OCTOPUS] = SK_OCTOPUS.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PENGUIN
	(TEXT("/Game/model/animals/penguin/penguin_swimming.penguin_swimming"));
	if (SK_PENGUIN.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_PENGUIN] = SK_PENGUIN.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_RABBIT
	(TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit"));
	if (SK_RABBIT.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_RABBIT] = SK_RABBIT.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SABERTOOTHTIGER
	(TEXT("/Game/model/animals/sabertoothTiger/sm_sabertoothTiger.sm_sabertoothTiger"));
	if (SK_SABERTOOTHTIGER.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_SABER_TOOTH_TIGER] = SK_SABERTOOTHTIGER.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SEAL
	(TEXT("/Game/model/animals/seal/seal1.seal1"));
	if (SK_SEAL.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_SEAL] = SK_SEAL.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SHEPHERD
	(TEXT("/Game/model/animals/shepherd/stylized_low_poly_german_shepherd.stylized_low_poly_german_shepherd"));
	if (SK_SHEPHERD.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_SHEPHERD] = SK_SHEPHERD.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SKUNK
	(TEXT("/Game/model/animals/skunk/low_poly_skunk.low_poly_skunk"));
	if (SK_SKUNK.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_SKUNK] = SK_SKUNK.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SLOTH
	(TEXT("/Game/model/animals/sloth/sm_sloth.sm_sloth"));
	if (SK_SLOTH.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_SLOTH] = SK_SLOTH.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SQUID
	(TEXT("/Game/model/animals/squid/Squid1.Squid1"));
	if (SK_SQUID.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_SQUID] = SK_SQUID.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STARFISH
	(TEXT("/Game/model/animals/starFish/star_fish.star_fish"));
	if (SK_STARFISH.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_STAR_FISH] = SK_STARFISH.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_STINGRAY
	(TEXT("/Game/model/animals/stingRay/sting_ray.sting_ray"));
	if (SK_STINGRAY.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_STINGRAY] = SK_STINGRAY.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WHALE
	(TEXT("/Game/model/animals/whale/whale1.whale1"));
	if (SK_WHALE.Succeeded()) {
		monsterMeshMap[EAnimal::ANIMAL_WHALE] = SK_WHALE.Object;
	}
}

void AReturnTriggerVolume::SpawnReturnEffect(TArray<EAnimal> Array)
{
    if (EffectsReturn)
    {
        FVector SpawnLocation = GetActorLocation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            EffectsReturn,
            SpawnLocation,
            GetActorRotation()
        );
    }

	// Spawn the first monster immediately
	if (Array.Num() > 0)
	{
		SpawnSingleMonster(Array[0], 0);
	}
	
	// Spawn monsters sequentially
	for (int32 Index = 1; Index < Array.Num(); ++Index)
	{
		EAnimal Animal = Array[Index];
		FTimerHandle SpawnTimerHandle;
		FTimerDelegate SpawnTimerDelegate;
        
		SpawnTimerDelegate.BindLambda([this, Animal, Index]()
		{
			SpawnSingleMonster(Animal, Index);
		});

		// Schedule the spawn with a delay based on the index
		float SpawnDelay = Index * 0.5f; // 0.5 seconds between each spawn, adjust as needed
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, SpawnTimerDelegate, SpawnDelay, false);
	}
}

void AReturnTriggerVolume::SpawnSingleMonster(EAnimal Animal, int32 Index)
{
	auto It = monsterMeshMap.find(Animal);
    if (It != monsterMeshMap.end())
    {
        USkeletalMeshComponent* Mesh = NewObject<USkeletalMeshComponent>(this);
        if (Mesh)
        {
            Mesh->SetSkeletalMesh(It->second);
            Mesh->RegisterComponent();
            Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

            // Set Initial Position at the center of the sphere
            FVector centerLocation = GetActorLocation();
            Mesh->SetWorldLocation(centerLocation);

            // Set Initial scale (adjust as needed)
            FVector OriginalBoundsExtent = Mesh->Bounds.BoxExtent;
            float MaxExtent = FMath::Max3(OriginalBoundsExtent.X, OriginalBoundsExtent.Y, OriginalBoundsExtent.Z);
            if (MaxExtent > 0)
            {
                float ScaleFactor = 150.0f / MaxExtent;
                Mesh->SetWorldScale3D(FVector(ScaleFactor, ScaleFactor, ScaleFactor));
            }

            // Initialize animation data
            FMeshAnimationData& AnimData = MeshAnimationMap[Mesh];
            AnimData.TotalTime = 0.0f;
            AnimData.Index = Index; // Store the index for use in animation

            // Start the ascension animation
            FTimerDelegate AnimationTimerDelegate;
            AnimationTimerDelegate.BindUFunction(this, FName("AnimateAnimalMesh"), Mesh);
            GetWorld()->GetTimerManager().SetTimer(AnimData.AnimationTimerHandle, AnimationTimerDelegate, 0.016f, true);

            // Schedule mesh destruction
            FTimerDelegate DestroyTimerDelegate;
            DestroyTimerDelegate.BindLambda([this, Mesh]()
            {
                if (IsValid(Mesh))
                {
                    Mesh->DestroyComponent();
                }
                MeshAnimationMap.erase(Mesh);
            });
            float DestroyDelay = 2.5f + (Index * 0.5f); // Adjust destroy timing based on spawn order
            GetWorld()->GetTimerManager().SetTimer(AnimData.DestroyTimerHandle, DestroyTimerDelegate, DestroyDelay, false);
        }
    }
}

void AReturnTriggerVolume::AnimateAnimalMesh(USkeletalMeshComponent* Mesh)
{
    if (!IsValid(Mesh)) return;
    
    auto It = MeshAnimationMap.find(Mesh);
    if (It == MeshAnimationMap.end()) return;

    FMeshAnimationData& AnimData = It->second;
    AnimData.TotalTime += GetWorld()->GetDeltaSeconds();

    FVector StartLocation = GetActorLocation();
    FVector EndLocation = StartLocation + FVector(0.0f, 0.0f, 750.0f * 5);

    // Calculate progress based on time, not position
    float AnimationDuration = 3.0f;
    float Progress = FMath::Clamp(AnimData.TotalTime / AnimationDuration, 0.0f, 1.0f);

    // Apply easing function for non-linear movement
    float EasedProgress = FMath::SmoothStep(0.0f, 1.0f, Progress);

    // Calculate new location
    FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, EasedProgress);

    // Apply additional vertical movement
    NewLocation.Z += FMath::Sin(Progress * PI) * 100.0f;

    // Add some horizontal movement, offset by index to create a spiral effect
    float HorizontalOffset = FMath::Sin(Progress * PI * 2) * (100.0f + AnimData.Index * 20.0f);
    float Angle = AnimData.TotalTime * 2 + AnimData.Index * (PI / 4); // Offset angle based on index
    NewLocation.X += HorizontalOffset * FMath::Cos(Angle);
    NewLocation.Y += HorizontalOffset * FMath::Sin(Angle);

    // Set new location
    Mesh->SetWorldLocation(NewLocation);

    // Rotate the mesh
    FRotator CurrentRotation = Mesh->GetComponentRotation();
    FRotator NewRotation = CurrentRotation + FRotator(0, 4, 0);
    Mesh->SetWorldRotation(NewRotation);

    // If the animation is complete, stop it
    if (Progress >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(AnimData.AnimationTimerHandle);
    }
}
