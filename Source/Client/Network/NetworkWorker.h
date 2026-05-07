// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#include "RecvBuffer.h"

class PacketSession;

struct CLIENT_API FPacketHeader
{
	FPacketHeader() : PacketID(0), PacketSize(0)
	{
	}

	FPacketHeader(uint16 PacketID, uint16 PacketSize) : PacketID(PacketID), PacketSize(PacketSize)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
	{
		Ar << Header.PacketID;
		Ar << Header.PacketSize;
		return Ar;
	}

	uint16 PacketID;
	uint16 PacketSize;
};

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 RecvWorker                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/
class CLIENT_API RecvWorker : public FRunnable
{
public:
	enum { BUFFER_SIZE = 0x10000 };

	RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session);
	RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, TSharedPtr<SslObject> Ssl);
	~RecvWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	void StartThread();
	void Destroy();

	RecvBuffer& GetDecBuffer() { return DecBuffer_; }
	virtual RecvBuffer& GetEncBuffer() { return DecBuffer_; }

	virtual uint8 Decrypt(RecvBuffer& encBuffer, RecvBuffer& decBuffer) { return 1; }

public:
	SslObjectRef SslRef;
	bool Recv();
	uint32 OnRecv(BYTE* buffer, uint32 len);

protected:
	FRunnableThread* Thread = nullptr;
	RecvBuffer DecBuffer_;
	bool Running = true;

	FSocket* Socket;
	TWeakPtr<PacketSession> sessionRef;
};

class CLIENT_API TLSRecvWorker : public RecvWorker
{
public:
	TLSRecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, SslObjectRef Ssl);
	virtual RecvBuffer& GetEncBuffer() override { return EncBuffer_; }
	virtual uint8 Decrypt(RecvBuffer& EncBuffer, RecvBuffer& DecBuffer) override;

protected:
	RecvBuffer EncBuffer_;
};

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 SendWorker                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/
class CLIENT_API SendWorker : public FRunnable
{
public:
	SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session);
	SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, TSharedPtr<SslObject> Ssl);
	~SendWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	bool SendPacket(SendBufferRef sendBuffer);
	virtual bool Encrypt(SendBufferRef& decBuffer, SendBufferRef& encBuffer);
	
	void StartThread();
	void Destroy();

private:
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool Running = true;

	FSocket* Socket;
	TWeakPtr<PacketSession> SessionRef;
	TSharedPtr<SslObject> SslRef;
};

class CLIENT_API TLSSendWorker : public SendWorker
{
public:
	TLSSendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, SslObjectRef Ssl);
	virtual bool Encrypt(SendBufferRef& decBuffer, SendBufferRef& encBuffer);
};
