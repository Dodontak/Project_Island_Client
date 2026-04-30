// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketSession.h"
#include "NetworkWorker.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"

PacketSession::PacketSession(class FSocket* Socket) : Socket(Socket)
{
}

PacketSession::~PacketSession()
{
	Disconnect();
}

void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void PacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}
}

void PacketSession::HandleRecvPackets()
{
	while (true)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
			break;
		// TODO
		//ServerPacketHandler::HandlePacket(Packet);
		Protocol::C_CHAT pkt;
		pkt.set_msg("hi i am unreal client!");
		SendPacket(ServerPacketHandler::MakeSendBuffer(pkt));
	}
}

void PacketSession::SendPacket(SendBufferRef SendBuffer)
{
	bool status = SendPacketQueue.Enqueue(SendBuffer);
}
