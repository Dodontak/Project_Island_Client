// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "Client.h"
// #include "SslObject.h"
//
// class UClientGameInstance;
//
// class CLIENT_API PacketSession : public TSharedFromThis<PacketSession>
// {
// public:
// 	PacketSession(FSocket* Socket, SSL_CTX* CTX);
// 	~PacketSession();
//
// 	void Run();
// 	void Disconnect();
//
// 	void HandleRecvPackets();
//
// 	void SendPacket(SendBufferRef SendBuffer);
//
// public: //TLS
// 	void TLSConnect();
//
// public:
// 	FSocket* Socket;
// 	TSharedPtr<SslObject> SslRef;
// 	FCriticalSection Mutex;
// 	TAtomic<bool> TLSConnected = false;
//
// 	TSharedPtr<class RecvWorker> RecvWorkerThread;
// 	TSharedPtr<class SendWorker> SendWorkerThread;
// 	TQueue<TArray<uint8>> RecvPacketQueue;
// 	TQueue<SendBufferRef> SendPacketQueue;
// };
