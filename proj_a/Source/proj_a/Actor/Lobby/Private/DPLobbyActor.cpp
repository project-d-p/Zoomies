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

    /* -------------------------- fox ---------------------------- */
    foxComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("foxComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_FOXMESH
    (TEXT("/Game/model/animals/fox_2/fox.fox"));
    if (SK_FOXMESH.Succeeded()) {
        foxComponent->SetSkeletalMesh(SK_FOXMESH.Object);
        foxComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
        foxComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    }

    foxComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> FOXANIMATION
    (TEXT("/Game/model/animals/fox_2/hit.hit"));
    if (FOXANIMATION.Succeeded()) {
        foxAnim = FOXANIMATION.Object;
        foxComponent->AnimationData.AnimToPlay = foxAnim;
        foxComponent->AnimationData.SavedPlayRate = 4.5f;
    }

    arrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowComponent"));
    arrowComponent->SetupAttachment(foxComponent);
    arrowComponent->SetRelativeLocation(FVector(400.f, 0.f, 0.f));
    arrowComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
    arrowComponent->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

    //foxAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("audioComponent"));
    //foxAudioComponent->SetupAttachment(foxComponent);

  /*  foxAudioComponent->bAutoActivate = false;
    static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue
    (TEXT("/Game/sounds/background/background_ingame_Cue.background_ingame_Cue"));
    if (SoundCue.Succeeded()) {
        foxAudioComponent->SetSound(SoundCue.Object);
    }
    static ConstructorHelpers::FObjectFinder<USoundAttenuation> AttenuationSettings
    (TEXT("/Game/sounds/dp_animalSoundAttenuation.dp_animalSoundAttenuation"));
    if (AttenuationSettings.Succeeded()) {
        foxAudioComponent->AttenuationSettings = AttenuationSettings.Object;
    }*/

    FAnimalData foxData;
    foxData.meshComponent = foxComponent;
    foxData.offset = arrowComponent->GetForwardVector() * 20;
    animalComponents.Add(foxData);

    /* -------------------------- giraffe ---------------------------- */
    giraffeComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("giraffeComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFEMESH
    (TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly"));
    if (SK_GIRAFFEMESH.Succeeded()) {
        giraffeComponent->SetSkeletalMesh(SK_GIRAFFEMESH.Object);
        giraffeComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
        giraffeComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
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
    giraffeData.offset = arrowComponent->GetForwardVector() * 10;
    animalComponents.Add(giraffeData);

    /* -------------------------- racoon ---------------------------- */
    racoonComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("racoonComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_RACOONMESH
    (TEXT("/Game/model/animals/racoon/low-poly_racoon_run_animation.low-poly_racoon_run_animation"));
    if (SK_RACOONMESH.Succeeded()) {
        racoonComponent->SetSkeletalMesh(SK_RACOONMESH.Object);
        racoonComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
        racoonComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    }

    racoonComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> RACOONANIMATION
    (TEXT("/Game/model/animals/racoon/low-poly_racoon_run_animation_Anim.low-poly_racoon_run_animation_Anim"));
    if (RACOONANIMATION.Succeeded()) {
        racoonAnim = RACOONANIMATION.Object;
        racoonComponent->AnimationData.AnimToPlay = racoonAnim;
        //racoonComponent->AnimationData.SavedPlayRate = 1.f;
    }

    FAnimalData racconData;
    racconData.meshComponent = racoonComponent;
    racconData.offset = arrowComponent->GetForwardVector() * 17.5;
    animalComponents.Add(racconData);

    /* -------------------------- rabbit ---------------------------- */
    //rabbitComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("rabbitComponent"));

    //static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_RABBITMESH
    //(TEXT("/Game/model/animals/rabbit/low_poly_rabbit.low_poly_rabbit"));
    //if (SK_RABBITMESH.Succeeded()) {
    //    rabbitComponent->SetSkeletalMesh(SK_RABBITMESH.Object);
    //    rabbitComponent->SetRelativeScale3D(FVector(0.13f, 0.13f, 0.13f));
    //    rabbitComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    //}

    //rabbitComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    //static ConstructorHelpers::FObjectFinder<UAnimSequence> RABBITANIMATION
    //(TEXT("/Game/model/animals/rabbit/Armature_001_Run.Armature_001_Run"));
    //if (RABBITANIMATION.Succeeded()) {
    //    rabbitAnim = RABBITANIMATION.Object;
    //    rabbitComponent->AnimationData.AnimToPlay = rabbitAnim;
    //    //rabbitComponent->AnimationData.SavedPlayRate = 1.f;
    //}

    //FAnimalData rabbitData;
    //rabbitData.meshComponent = rabbitComponent;
    //rabbitData.offset = arrowComponent->GetForwardVector() * 15;
    //animalComponents.Add(rabbitData);

    /* -------------------------- crab ---------------------------- */
    crabComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("crabComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CRABMESH
    (TEXT("/Game/model/animals/crab/crab.crab"));
    if (SK_CRABMESH.Succeeded()) {
        crabComponent->SetSkeletalMesh(SK_CRABMESH.Object);
        crabComponent->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
        crabComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    }

    crabComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> CRABANIMATION
    (TEXT("/Game/model/animals/crab/Walk.Walk"));
    if (CRABANIMATION.Succeeded()) {
        crabAnim = CRABANIMATION.Object;
        crabComponent->AnimationData.AnimToPlay = crabAnim;
        //crabComponent->AnimationData.SavedPlayRate = 1.f;
    }

    FAnimalData crabData;
    crabData.meshComponent = crabComponent;
    crabData.offset = arrowComponent->GetForwardVector() * 3;
    animalComponents.Add(crabData);

    /* -------------------------- turtle ---------------------------- */
    turtleComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("turtleComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_TURTLEMESH
    (TEXT("/Game/model/animals/turtle/Turtle.Turtle"));
    if (SK_TURTLEMESH.Succeeded()) {
        turtleComponent->SetSkeletalMesh(SK_TURTLEMESH.Object);
        turtleComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
        turtleComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    }

    turtleComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> TURTLEANIMATION
    (TEXT("/Game/model/animals/turtle/Swim.Swim"));
    if (TURTLEANIMATION.Succeeded()) {
        turtleAnim = TURTLEANIMATION.Object;
        turtleComponent->AnimationData.AnimToPlay = turtleAnim;
        //turtleComponent->AnimationData.SavedPlayRate = 1.f;
    }

    FAnimalData turtleData;
    turtleData.meshComponent = turtleComponent;
    turtleData.offset = arrowComponent->GetForwardVector() * 8;
    animalComponents.Add(turtleData);

    /* -------------------------- lion ---------------------------- */
    lionComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("lionComponent"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LIONMESH
    (TEXT("/Game/model/animals/lion/low_poly_lion.low_poly_lion"));
    if (SK_LIONMESH.Succeeded()) {
        lionComponent->SetSkeletalMesh(SK_LIONMESH.Object);
        lionComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
        lionComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    }

    lionComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    static ConstructorHelpers::FObjectFinder<UAnimSequence> LIONANIMATION
    (TEXT("/Game/model/animals/lion/Armature_LionRun.Armature_LionRun"));
    if (LIONANIMATION.Succeeded()) {
        lionAnim = LIONANIMATION.Object;
        lionComponent->AnimationData.AnimToPlay = lionAnim;
        lionComponent->AnimationData.SavedPlayRate = 0.6f;
    }

    FAnimalData lionData;
    lionData.meshComponent = lionComponent;
    lionData.offset = arrowComponent->GetForwardVector() * 23;
    animalComponents.Add(lionData);

    /* -------------------------- box ---------------------------- */
    boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent"));
    boxComponent->SetupAttachment(RootComponent);
    boxComponent->SetRelativeLocation(FVector(0.f, 500.f, 0.f));
}

// Called when the game starts or when spawned
void ADPLobbyActor::BeginPlay()
{
	Super::BeginPlay();

    initialLocation = GetActorLocation();
    //foxAudioComponent->Play();
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
            if (foxComponent == animal.meshComponent)
                newLocation.Z = 38.f;
            if (giraffeComponent == animal.meshComponent)
                newLocation.Z = 55.f;
            if (racoonComponent == animal.meshComponent)
                newLocation.Z = 22.f;
            //if (rabbitComponent == animal.meshComponent)
             //   newLocation.Z = 15.f;
            if (crabComponent == animal.meshComponent)
                newLocation.Z = 10.f;
            if (turtleComponent == animal.meshComponent)
                newLocation.Z = 28.f;
            if (lionComponent == animal.meshComponent)
                newLocation.Z = 55.f;
            animal.meshComponent->SetRelativeLocation(newLocation);
        }
    }
}

