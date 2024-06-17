// Fill out your copyright notice in the Description page of Project Settings.

// #include "DPAnimInstance.h"
#include "proj_a/Component/AnimNotify/Public/DPAnimInstance.h"

#include "Kismet/GameplayStatics.h"

UDPAnimInstance::UDPAnimInstance()
{
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset
    (TEXT("/Game/sounds/effect/click_Cue.click_Cue"));
    if (SoundAsset.Succeeded()) {
        sound = SoundAsset.Object;
    }
    else
        sound = nullptr;
}

void UDPAnimInstance::AnimNotify_Footstep()
{
    UE_LOG(LogTemp, Warning, TEXT("Animation_Footstep"));
    UGameplayStatics::PlaySound2D(GetWorld(), sound);
}

//void UDPAnimInstance::AnimNotify_foot()
//{
    //UE_LOG(LogTemp, Warning, TEXT("Animation_Foot"));
    //UGameplayStatics::PlaySound2D(GetWorld(), sound);
//}

void UDPAnimInstance::AnimNotify_foot_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Animation_Foot"));
    UGameplayStatics::PlaySound2D(GetWorld(), sound);
}
