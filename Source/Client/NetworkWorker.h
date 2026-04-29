// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client.h"

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
	RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session);
	~RecvWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	
	void Destroy();
	
private:
	bool ReceivePacket(TArray<uint8>& OutPayload);
	bool ReceiveDesiredBytes(uint8* Results, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool Running = true;
	
	FSocket* Socket;
	TWeakPtr<PacketSession> SessionRef;
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
	~SendWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	bool SendPacket(SendBufferRef SendBuffer);

	void Destroy();

private:
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool Running = true;

	FSocket* Socket;
	TWeakPtr<PacketSession> SessionRef;
};