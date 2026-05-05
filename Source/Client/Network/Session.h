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
	virtual uint32 OnRecv(BYTE* buffer, uint32 len);

	bool ConnectToGameServer();
	virtual void OnConnect();
	virtual void TLSConnect();

	virtual void Run();

	void HandleRecvPackets();

	TAtomic<bool> IsRunning;
	TAtomic<bool> IsTCPConnected;

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
	virtual ~TLSSession();
	virtual void Run() final;

	virtual void TLSConnect() final;
	void HandshakeSend();
	void HandshakeRecv();

public:
	TAtomic<bool> IsTLSConnected;
	TSharedPtr<SslObject> SslRef;
};
