// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPPlayerController.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
}
