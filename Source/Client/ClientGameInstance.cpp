// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientGameInstance.h"

// OpenSSL의 UI 구조체 이름을 잠시 바꿔서 언리얼의 namespace UI와 충돌을 피함
#define UI UI_ST
#include <openssl/ssl.h>
#include <openssl/err.h>
#undef UI
// --- OpenSSL 충돌 방지 처리 끝 ---

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "Session.h"
#include "PacketSession.h"

void UClientGameInstance::Init()
{
	const SSL_METHOD* Method = TLS_client_method();
	Ctx = SSL_CTX_new(Method);
}

void UClientGameInstance::ConnectToGameServer()
{
	UE_LOG(LogTemp, Display, TEXT("message"));
	FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(
		TEXT("Stream"),TEXT("Client Socket"));;

	FString IpAddress = "127.0.0.1";
	int16 Port = 7777;
	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);
	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Connecting To Server"));

	bool Ret = Socket->Connect(*InternetAddr);
	if (Ret)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Connection Successed"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection Failed"));
	}
}

void UClientGameInstance::DisconnectFromGameServer()
{
}

void UClientGameInstance::HandleRecvPackets()
{
	if (GameServerSession == nullptr)
		return;
	GameServerSession->HandleRecvPackets();
}

void UClientGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	// if (Socket == nullptr || GameServerSession == nullptr)
	// 	return;
	//
	// GameServerSession->SendPacket(SendBuffer);
}
