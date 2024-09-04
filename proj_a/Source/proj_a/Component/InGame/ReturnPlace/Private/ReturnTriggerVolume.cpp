#include "ReturnTriggerVolume.h"

#include "DPCharacter.h"
#include "FNetLogger.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "PathManager.h"
#include "proj_a.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"

AReturnTriggerVolume::AReturnTriggerVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and configure the box component
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// TriggerSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerSphere->SetCollisionResponseToChannel(ECC_PlayerChannel, ECollisionResponse::ECR_Overlap);
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

AReturnTriggerVolume::~AReturnTriggerVolume()
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (auto& Pair : MeshAnimationMap)
		{
			FMeshAnimationData& AnimData = Pair.second;
			World->GetTimerManager().ClearTimer(AnimData.AnimationTimerHandle);
			World->GetTimerManager().ClearTimer(AnimData.DestroyTimerHandle);
		}
		for (FTimerHandle* Handle : SpawnTimerHandles)
		{
			World->GetTimerManager().ClearTimer(*Handle);
			delete Handle;
		}
	}
	MeshAnimationMap.clear();
	SpawnTimerHandles.Empty();
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
	struct FAnimalInfo
	{
		EAnimal AnimalType;
		FString PathSuffix;
	};

	TArray<FAnimalInfo> AnimalInfos = {
		{EAnimal::ANIMAL_CRAB, "CRAB"},
		{EAnimal::ANIMAL_DOLPHIN, "DOLPHIN"},
		{EAnimal::ANIMAL_EEL, "EEL"},
		{EAnimal::ANIMAL_ELEPHANT, "ELEPHANT"},
		{EAnimal::ANIMAL_FOX, "FOX"},
		{EAnimal::ANIMAL_GIRAFFE, "GIRAFFE"},
		{EAnimal::ANIMAL_LION, "LION"},
		{EAnimal::ANIMAL_LOBSTER, "LOBSTER"},
		{EAnimal::ANIMAL_MAMMOTH, "MAMMOTH"},
		{EAnimal::ANIMAL_OCTOPUS, "OCTOPUS"},
		{EAnimal::ANIMAL_PENGUIN, "PENGUIN"},
		{EAnimal::ANIMAL_SABER_TOOTH_TIGER, "SABER_TOOTH_TIGER"},
		{EAnimal::ANIMAL_SEAL, "SEAL"},
		{EAnimal::ANIMAL_SKUNK, "SKUNK"},
		{EAnimal::ANIMAL_SLOTH, "SLOTH"},
		{EAnimal::ANIMAL_SQUID, "SQUID"},
		{EAnimal::ANIMAL_STAR_FISH, "STAR_FISH"},
		{EAnimal::ANIMAL_STINGRAY, "STINGRAY"},
		{EAnimal::ANIMAL_WHALE, "WHALE"},
		{EAnimal::ANIMAL_CHAMELEON, "CHAMELEON"},
		{EAnimal::ANIMAL_HORSE, "HORSE"}
	};

	for (const auto& AnimalInfo : AnimalInfos)
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder
		(PathManager::GetMonsterPath(AnimalInfo.AnimalType));
        
		if (SkeletalMeshFinder.Succeeded())
		{
			monsterMeshMap[AnimalInfo.AnimalType] = SkeletalMeshFinder.Object;
		}
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
		FTimerHandle* SpawnTimerHandle = new FTimerHandle();
		FTimerDelegate SpawnTimerDelegate;
        
		TWeakObjectPtr<AReturnTriggerVolume> WeakThis(this);
        
		SpawnTimerDelegate.BindLambda([WeakThis, Animal, Index, &SpawnTimerHandle]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->SpawnSingleMonster(Animal, Index);
				WeakThis->SpawnTimerHandles.Remove(SpawnTimerHandle);
				delete SpawnTimerHandle;
			}
		});
		float SpawnDelay = Index * 0.5f;
		GetWorld()->GetTimerManager().SetTimer(*SpawnTimerHandle, SpawnTimerDelegate, SpawnDelay, false);
		SpawnTimerHandles.Add(SpawnTimerHandle);
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

        	TWeakObjectPtr<AReturnTriggerVolume> WeakThis(this);
            // Start the ascension animation
            FTimerDelegate AnimationTimerDelegate;
			// AnimationTimerDelegate.BindUFunction(this, FName("AnimateAnimalMesh"), Mesh);
        	AnimationTimerDelegate.BindLambda([WeakThis, Mesh]()
        	{
        		if (WeakThis.IsValid() && IsValid(Mesh))
        		{
        			WeakThis->AnimateAnimalMesh(Mesh);
        		}
        	});
            GetWorld()->GetTimerManager().SetTimer(AnimData.AnimationTimerHandle, AnimationTimerDelegate, 0.016f, true);

        	// Schedule mesh destruction
        	FTimerDelegate DestroyTimerDelegate;
        	DestroyTimerDelegate.BindLambda([WeakThis, Mesh]()
			{
				if (WeakThis.IsValid() && IsValid(Mesh))
				{
					Mesh->DestroyComponent();
					WeakThis->MeshAnimationMap.erase(Mesh);
				}
			});
        	float DestroyDelay = 2.5f + (Index * 0.5f);
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
    if (Progress < 1.0f)
    {
    	return ;
    }
    if (AnimData.AnimationTimerHandle.IsValid())
    {
		GetWorld()->GetTimerManager().ClearTimer(AnimData.AnimationTimerHandle);
    }
}
