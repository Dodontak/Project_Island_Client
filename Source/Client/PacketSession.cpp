// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "PacketSession.h"
// #include "NetworkWorker.h"
// #include "Protocol.pb.h"
// #include "SslObject.h"
// #include "ClientGameInstance.h"
// #include "ServerPacketHandler.h"
//
// PacketSession::PacketSession(FSocket* Socket, SSL_CTX* CTX) : Socket(Socket)
// {
// 	ServerPacketHandler::Init();
// 	SslRef = MakeShared<SslObject>();
// 	SslRef->Init(CTX);
// }
//
// PacketSession::~PacketSession()
// {
// 	Disconnect();
// }
//
// void PacketSession::Run()
// {
// 	// RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
// 	// SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
// }
//
// void PacketSession::Disconnect()
// {
// 	if (RecvWorkerThread)
// 	{
// 		RecvWorkerThread->Destroy();
// 		RecvWorkerThread = nullptr;
// 	}
//
// 	if (SendWorkerThread)
// 	{
// 		SendWorkerThread->Destroy();
// 		SendWorkerThread = nullptr;
// 	}
// }
//
// void PacketSession::TLSConnect()
// {
// 	SslStatus status = SslRef->Connect();
//
// 	switch (status)
// 	{
// 	case SslStatus::Ok:
// 		// TLS연결완료
// 		break;
// 	case SslStatus::WantRead:
// 		//wbio에 보낼 데이터가 생겼으면 보냄.
// 		break;
// 	case SslStatus::WantWrite:
// 		//wbio가 꽉 차서 Accept가 진행되지 못한 경우. wbio에 있는 데이터를 Send한다.
// 		break;
// 	default:
// 		// 에러 발생함. 연결 종료.
// 		break;
// 	}
// }
//
// void PacketSession::HandleRecvPackets()
// {
// 	while (true)
// 	{
// 		TArray<uint8> Packet;
// 		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
// 			break;
// 		DeferredFunc Func;
// 		if (ServerPacketHandler::PacketHandler(OUT Func, AsShared(), Packet.GetData(), Packet.Num()))
// 			Func();
// 	}
// }
//
// void PacketSession::SendPacket(SendBufferRef SendBuffer)
// {
// 	SendPacketQueue.Enqueue(SendBuffer);
// }
