// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "Marshaller.h"
#include "SocketManager.h"

int32 UNetComp::sendTCP(const FVector2D &input)
{
	USocketManager *socketManager = USocketManager::getInstance();
	FSocket *sock = socketManager->getTCPSocket();
	
	TArray<uint8> bData = Marshaller::serializeData(input);
	return socketManager->send(sock, bData.GetData(), bData.Num());
}

int32 UNetComp::sendUDP(const FVector2D &input)
{
	USocketManager *socketManager = USocketManager::getInstance();
	FSocket *sock = socketManager->getUDPSocket();
	
	TArray<uint8> bData = Marshaller::serializeData(input);
	return socketManager->send(sock, bData.GetData(), bData.Num());
}
