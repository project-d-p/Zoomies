// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "Marshaller.h"
#include "SocketManager.h"
#include "NetQueue.h"

FNetworkTask* UNetComp::TCPtask = nullptr;
FNetworkTask* UNetComp::UDPtask = nullptr;

// TODO: end 추가

int32 UNetComp::inputTCP(const FVector2D &input, const int32 &type)
{
	if (!TCPtask)
	{
		TCPtask = new FNetworkTask(true);
	}
	if (!InputQueue.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Input is not empty"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Input is empty"));
	}
	FInputData inputData;
	inputData.InputVector = input;
	inputData.Type = type;
	InputQueue.Enqueue(inputData);
	
	return 1;
	// USocketManager *socketManager = USocketManager::getInstance();
	// FSocket *sock = socketManager->getTCPSocket();
	//
	// TArray<uint8> bData = Marshaller::serializeData(input);
	// return socketManager->send(sock, bData.GetData(), bData.Num());
}

int32 UNetComp::inputUDP(const FVector2D &input)
{
	USocketManager *socketManager = USocketManager::getInstance();
	FSocket *sock = socketManager->getUDPSocket();
	
	TArray<uint8> bData = Marshaller::serializeData(input);
	return socketManager->send(sock, bData.GetData(), bData.Num());
}
