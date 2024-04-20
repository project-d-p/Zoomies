// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "NetComp.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_A_API UNetComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// XXX: 사용성을 위해서 중복을 허용. 추후 다른 아이디어가 있다면 수정
	static int32 sendTCP(const FVector2D &input);
	static int32 sendUDP(const FVector2D &input);
};
