#pragma once

#include "Client.h"

#define UI UI_ST
#include <openssl/ossl_typ.h>
#undef UI

#include "Containers/Deque.h"

class SslObject;

class CLIENT_API PacketSession : public TSharedFromThis<PacketSession>
{
	friend class UClientGameInstance;

public:
	PacketSession(FString IpAddress, uint32 Port);
	virtual ~PacketSession();

	bool ConnectToGameServer();
	virtual void TLSConnect();
	virtual void Run();

	void HandleRecvPackets();
	void SendPacket(SendBufferRef SendBuffer);

	TAtomic<bool> IsRunning;

	TSharedPtr<class RecvWorker> RecvWorkerThread;
	TSharedPtr<class SendWorker> SendWorkerThread;
	TQueue<TArray<uint8>> RecvPacketQueue;
	TDeque<SendBufferRef> SendDeque;

	FSocket* Socket;
	FString IpAddress;
	int16 Port;
};

class CLIENT_API TLSSession : public PacketSession
{
public:
	TLSSession(FString IpAddress, uint32 Port, SSL_CTX* CTX);
	virtual ~TLSSession() override;
	virtual void Run() override;

	virtual void TLSConnect() override;
	void HandshakeSend();
	void HandshakeRecv();

public:
	TSharedPtr<SslObject> SslRef;
};
