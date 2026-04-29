// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#include "Engine/GameInstance.h"
#include "ClientGameInstance.generated.h"

class FSocket;
class PacketSession;

UCLASS()
class CLIENT_API UClientGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();
	
	void SendPacket(SendBufferRef SendBuffer);
	
public:
	FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	
	TSharedPtr<PacketSession> GameServerSession;
};
