// Fill out your copyright notice in the Description page of Project Settings.

#include "NetComp.h"

#include "FTcpSendTask.h"
#include "FUdpSendTask.h"

// Type은 입력 데이터가 나눠질 수 있겠다는 생각에 추가한 변수
void UNetComp::InputTCP(const FVector2D& Input, const int32& Type)
{
	FInputData InputData;
	InputData.InputVector = Input;
	InputData.Type = Type;
	FTcpSendTask::InputQueue.Enqueue(InputData);
}

void UNetComp::InputUDP(const FVector2D& Input)
{
	FInputData InputData;
	InputData.InputVector = Input;
	InputData.Type = 0;
	FUdpSendTask::InputQueue.Enqueue(InputData);
}
