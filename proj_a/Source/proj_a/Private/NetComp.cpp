// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "Network/Marshaller.h"
#include "Network/SocketManager.h"

int32 UNetComp::sendInput(const FVector2D &input)
{
	TArray<uint8> bData = Marshaller::serializeData(input);
	return USocketManager::getInstance()->send(bData.GetData(), bData.Num());
}

