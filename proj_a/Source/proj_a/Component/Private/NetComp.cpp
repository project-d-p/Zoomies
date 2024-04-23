// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "FNetworkTask.h"

// TODO: end 추가

int32 UNetComp::inputTCP(const FVector2D &input, const int32 &type)
{
	FInputData inputData;
	inputData.InputVector = input;
	inputData.Type = type;
	FNetworkTask::InputQueue.Enqueue(inputData);
	
	return 1;
}

// int32 UNetComp::inputUDP(const FVector2D &input)
// {
// 	USocketManager *socketManager = USocketManager::getInstance();
// 	FSocket *sock = socketManager->getUDPSocket();
// 	
// 	TArray<uint8> bData = Marshaller::serializeData(input);
// 	return socketManager->send(sock, bData.GetData(), bData.Num());
// }
