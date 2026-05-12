// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientGameInstance.h"

// OpenSSL의 UI 구조체 이름을 잠시 바꿔서 언리얼의 namespace UI와 충돌을 피함
#define UI UI_ST
#include <openssl/ssl.h>
#undef UI
// --- OpenSSL 충돌 방지 처리 끝 ---

#include "PacketSession.h"
#include "Sockets.h"
#include "NetworkWorker.h"
#include "ServerPacketHandler.h"
#include "AuthSession.h"

void UClientGameInstance::Init()
{
	const SSL_METHOD* Method = TLS_client_method();
	Ctx = SSL_CTX_new(Method);
	ServerPacketHandler::Init();
}

void UClientGameInstance::Shutdown()
{
	Super::Shutdown();

	if (GameServerSession)
	{
		// 워커 스레드 먼저 종료
		if (GameServerSession->RecvWorkerThread)
		{
			GameServerSession->RecvWorkerThread->Destroy();
		}
		if (GameServerSession->SendWorkerThread)
		{
			GameServerSession->SendWorkerThread->Destroy();
		}

		// 소켓 닫기
		if (GameServerSession->Socket)
		{
			GameServerSession->Socket->Close();
		}

		GameServerSession = nullptr;
	}
	if (AuthServerSession)
	{
		// 워커 스레드 먼저 종료
		if (AuthServerSession->RecvWorkerThread)
		{
			AuthServerSession->RecvWorkerThread->Destroy();
		}
		if (AuthServerSession->SendWorkerThread)
		{
			AuthServerSession->SendWorkerThread->Destroy();
		}

		// 소켓 닫기
		if (AuthServerSession->Socket)
		{
			AuthServerSession->Socket->Close();
		}

		AuthServerSession = nullptr;
	}
}

void UClientGameInstance::ConnectToGameServer()
{
	AuthServerSession = MakeShared<AuthSession>("192.168.0.39", 4242, Ctx);
	AuthServerSession->ConnectToGameServer();
}

void UClientGameInstance::DisconnectFromGameServer()
{
}

void UClientGameInstance::HandleRecvPackets()
{
	if (GameServerSession != nullptr)
		GameServerSession->HandleRecvPackets();
	if (AuthServerSession != nullptr)
		AuthServerSession->HandleRecvPackets();
}

void UClientGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UClientGameInstance::LoginToAuthServer(FString Id, FString Password)
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->LoginToAuthServer(Id, Password);
}

void UClientGameInstance::SignUpValidCheck(bool SkipEmail, FString Email, FString Id, FString Password)
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->SignUpValidCheck(SkipEmail, Email, Id, Password);
}

void UClientGameInstance::RequestVerifyCode()
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->RequestVerifyCode();
}

void UClientGameInstance::SignUpVerifyEmailCode(FString Code)
{
	TSharedPtr<AuthSession> AuthServerSessionRef = StaticCastSharedPtr<AuthSession>(AuthServerSession);
	if (AuthServerSessionRef == nullptr)
		return;
	AuthServerSessionRef->SignUpVerifyEmailCode(Code);
}
