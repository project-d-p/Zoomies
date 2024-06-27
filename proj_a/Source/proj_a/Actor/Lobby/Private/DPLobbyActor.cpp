// Fill out your copyright notice in the Description page of Project Settings.


#include "DPLobbyActor.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"

// Sets default values
ADPLobbyActor::ADPLobbyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    foxComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("foxComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOXMESH
    (TEXT("/Game/model/animals/fox_2/fox.fox"));
    if (SK_FOXMESH.Succeeded()) {
        foxComponent->SetSkeletalMesh(SK_FOXMESH.Object);
        foxComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
        foxComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    }

    foxComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> FOXANIMATION
    (TEXT("/Game/model/animals/fox_2/hit.hit"));
    if (FOXANIMATION.Succeeded()) {
        foxAnim = FOXANIMATION.Object;
        foxComponent->AnimationData.AnimToPlay = foxAnim;
        foxComponent->AnimationData.SavedPlayRate = 3.5f;
    }

    arrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowComponent"));
    arrowComponent->SetupAttachment(foxComponent);
    arrowComponent->SetRelativeLocation(FVector(0.f, 400.f, 0.f));
    arrowComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    arrowComponent->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

    //foxAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));
    //foxAudioComponent->SetupAttachment(foxComponent);

    //static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue
    //(TEXT("/Game/sounds/background/background_ingame_Cue.background_ingame_Cue"));
    //if (SoundCue.Succeeded()) {
    //    foxAudioComponent->SetSound(SoundCue.Object);
    //    foxAudioComponent->Play();
    //}

    //static ConstructorHelpers::FObjectFinder<USoundAttenuation> AttenuationSettings
    //(TEXT("/Game/sounds/dp_animalSoundAttenuation.dp_animalSoundAttenuation"));
    //if (AttenuationSettings.Succeeded()) {
    //    foxAudioComponent->AttenuationSettings = AttenuationSettings.Object;
    //}

    FAnimalData foxData;
    foxData.meshComponent = foxComponent;
    foxData.offset = FVector(arrowComponent->GetForwardVector().Y,
        arrowComponent->GetForwardVector().X,
        arrowComponent->GetForwardVector().Z) * 20;
    animalComponents.Add(foxData);

    /* -------------------------- giraffe ---------------------------- */
    giraffeComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("giraffeComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFEMESH
    (TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly"));
    if (SK_GIRAFFEMESH.Succeeded()) {
        giraffeComponent->SetSkeletalMesh(SK_GIRAFFEMESH.Object);
        //giraffeComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
        giraffeComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    }

    giraffeComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> GIRAFFEANIMATION
    (TEXT("/Game/model/animals/giraffe/Run.Run"));
    if (GIRAFFEANIMATION.Succeeded()) {
        giraffeAnim = GIRAFFEANIMATION.Object;
        giraffeComponent->AnimationData.AnimToPlay = giraffeAnim;
        //giraffeComponent->AnimationData.SavedPlayRate = 1.f;
    }

    FAnimalData giraffeData;
    giraffeData.meshComponent = giraffeComponent;
    giraffeData.offset = FVector(arrowComponent->GetForwardVector().Y,
        arrowComponent->GetForwardVector().X,
        arrowComponent->GetForwardVector().Z) * 10;
    animalComponents.Add(giraffeData);





    boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent"));
    boxComponent->SetupAttachment(RootComponent);
    boxComponent->SetRelativeLocation(FVector(2000.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void ADPLobbyActor::BeginPlay()
{
	Super::BeginPlay();

    initialLocation = GetActorLocation();
}

// Called every frame
void ADPLobbyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    for (auto& Animal : animalComponents) {
        if (Animal.meshComponent) {
            Animal.meshComponent->AddLocalOffset(Animal.offset);
        }
    }

    CheckReach();
}

void ADPLobbyActor::CheckReach()
{
    for (auto& animal : animalComponents) {
        if (animal.meshComponent && animal.meshComponent->GetRelativeLocation().Y > boxComponent->GetRelativeLocation().Y) {
            FVector newLocation = initialLocation;
            newLocation.X = FMath::RandRange(-2000.f, 10.f);
            if (giraffeComponent == animal.meshComponent)
                newLocation.Z = 0.f;
            animal.meshComponent->SetRelativeLocation(newLocation);
        }
    }
}

