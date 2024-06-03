// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "FUdpSendTask.h"

void UNetComp::InputUDP(const FVector2D& Input)
{
	FInputData InputData;
	InputData.InputVector = Input;
	InputData.Type = 0;
	FUdpSendTask::InputQueue.Enqueue(InputData);
}
