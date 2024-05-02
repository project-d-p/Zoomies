// Fill out your copyright notice in the Description page of Project Settings.

#include "Marshaller.h"

TArray<uint8> Marshaller::SerializeMessage(const Message& Msg)
{
	const uint32_t MSG_SIZE = Msg.ByteSizeLong();

	TArray<uint8> buffer;
	buffer.AddUninitialized(sizeof(MSG_SIZE) + MSG_SIZE);

	memcpy(buffer.GetData(), &MSG_SIZE, sizeof(MSG_SIZE));

	Msg.SerializeToArray(buffer.GetData() + sizeof(MSG_SIZE), MSG_SIZE);

	return buffer;
}

Message Marshaller::DeserializeMessage(const TArray<uint8>& Buffer)
{
	uint32_t message_size;
	memcpy(&message_size, Buffer.GetData(), sizeof(message_size));

	Message msg;
	msg.ParseFromArray(Buffer.GetData() + sizeof(message_size), message_size);

	return msg;
}
