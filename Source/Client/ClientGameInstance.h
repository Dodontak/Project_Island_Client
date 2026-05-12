// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#define UI UI_ST
#include <openssl/ossl_typ.h>
#undef UI
#include "Engine/GameInstance.h"
#include "ClientGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSignUpValidCheck, bool, Success, FString, Email, FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSignUpVerifyCode, bool, Success, FString, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginToGameServer, bool, Success, FString, Reason);

class FSocket;
class PacketSession;

UCLASS()
class CLIENT_API UClientGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	UFUNCTION(BlueprintCallable)
	void LoginToAuthServer(FString Id, FString Password);

	UFUNCTION(BlueprintCallable)
	void SignUpValidCheck(bool SkipEmail, FString Email, FString Id, FString Password);

	UFUNCTION(BlueprintCallable)
	void RequestVerifyCode();

	UFUNCTION(BlueprintCallable)
	void SignUpVerifyEmailCode(FString Code);
	
	UPROPERTY(BlueprintAssignable)
	FOnSignUpValidCheck OnSignUpValidCheck;

	UPROPERTY(BlueprintAssignable)
	FOnSignUpVerifyCode OnSignUpVerifyCode;

	UPROPERTY(BlueprintAssignable)
	FOnLoginToGameServer OnLoginToGameServer;

public:
	SSL_CTX* Ctx = nullptr;
	TSharedPtr<PacketSession> GameServerSession = nullptr;
	TSharedPtr<PacketSession> AuthServerSession = nullptr;
};
