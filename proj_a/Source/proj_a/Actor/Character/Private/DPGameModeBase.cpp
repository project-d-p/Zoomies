// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPPlayerController.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
    //static ConstructorHelpers::FClassFinder<APawn> CHARACTER
    //(TEXT("/Game/blueprints/bp_character.bp_character_C"));
    //if (CHARACTER.Class != nullptr)
    //{
    //    DefaultPawnClass = CHARACTER.Class;
    //}

	PlayerControllerClass = ADPPlayerController::StaticClass();
}
