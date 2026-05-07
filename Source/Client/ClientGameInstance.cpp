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
}

void UClientGameInstance::ConnectToGameServer()
{
	GameServerSession = MakeShared<TLSSession>("127.0.0.1", 7777, Ctx);
	GameServerSession->ConnectToGameServer();
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
	if (GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}
