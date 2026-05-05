// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#define UI UI_ST
#include <openssl/ossl_typ.h>
#undef UI
#include "Engine/GameInstance.h"
#include "ClientGameInstance.generated.h"

class FSocket;
class PacketSession;

UCLASS()
class CLIENT_API UClientGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();
	
	void SendPacket(SendBufferRef SendBuffer);
	
public:
	SSL_CTX* Ctx = nullptr;
	TSharedPtr<PacketSession> GameServerSession;
};
