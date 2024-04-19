// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "NetComp.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_A_API UNetComp : public UActorComponent
{
	GENERATED_BODY()

public:
	static int32 sendInput(const FVector2D &input);
};
