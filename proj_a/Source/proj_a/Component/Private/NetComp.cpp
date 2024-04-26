// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "FTcpSendTask.h"
#include "FUdpSendTask.h"

// TODO: end 추가

int32 UNetComp::inputTCP(const FVector2D &input, const int32 &type)
{
	FInputData inputData;
	inputData.InputVector = input;
	inputData.Type = type;
	FTcpSendTask::InputQueue.Enqueue(inputData);
	
	return 1;
}

int32 UNetComp::inputUDP(const FVector2D &input)
{
	FInputData inputData;
	inputData.InputVector = input;
	inputData.Type = 0;
	FUdpSendTask::InputQueue.Enqueue(inputData);
	
	return 1;
}
