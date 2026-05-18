#include "PacketSession.h"

#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"
#include "Client.h"
#include "NetworkWorker.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "ServerPacketHandler.h"
#include "SslObject.h"

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                PacketSession                                 |
|                                                                              |
\*----------------------------------------------------------------------------*/
PacketSession::PacketSession(FString IpAddress, uint32 Port)
	: IpAddress(IpAddress), Port(Port)
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"),TEXT("Client Socket"));
}

PacketSession::~PacketSession()
{
}

bool PacketSession::ConnectToGameServer()
{
	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);
	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Connecting To Server"));

	if (Socket->Connect(*InternetAddr))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Connection Successed"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection Failed"));
		return false;
	}
	TLSConnect();
	return true;
}

void PacketSession::TLSConnect()
{
	Run();
}

void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
	RecvWorkerThread->StartThread();
	SendWorkerThread->StartThread();
}

void PacketSession::HandleRecvPackets()
{
	TArray<uint8> PacketBuffer;

	if (RecvPacketQueue.Dequeue(PacketBuffer))
	{
		DeferredFunc Func;
		ServerPacketHandler::PacketHandler(OUT Func, AsShared(), PacketBuffer.GetData(), PacketBuffer.Num());
		if (Func)
			Func();
	}
}

void PacketSession::SendPacket(SendBufferRef SendBuffer)
{
	SendDeque.PushFirst(SendBuffer);
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 TLSSession                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

TLSSession::TLSSession(FString IpAddress, uint32 Port, SSL_CTX* CTX) : PacketSession(IpAddress, Port)
{
	SslRef = MakeShared<SslObject>();
	SslRef->Init(CTX);
}

TLSSession::~TLSSession()
{
}

void TLSSession::Run()
{
	TSharedPtr<TLSRecvWorker> TLSRW = MakeShared<TLSRecvWorker>(Socket, AsShared(), SslRef);
	RecvWorkerThread = TLSRW;
	TSharedPtr<TLSSendWorker> TLSSW = MakeShared<TLSSendWorker>(Socket, AsShared(), SslRef);
	SendWorkerThread = TLSSW;

	RecvWorkerThread->StartThread();
	SendWorkerThread->StartThread();
}

//TLS연결하고, 성공하면 recvworker, sendworker 시작.
void TLSSession::TLSConnect()
{
	switch (SslRef->Connect())
	{
	case SslStatus::Ok:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TLS Successed"));
		HandshakeSend();
		Run();
		break;
	case SslStatus::WantRead:
		//wbio에 보낼 데이터가 생겼으면 보내고 recv
		HandshakeSend();
		HandshakeRecv();
		break;
	case SslStatus::WantWrite:
		//wbio가 꽉 차서 Accept가 진행되지 못한 경우. wbio에 있는 데이터를 Send한다.
		HandshakeSend();
		break;
	default:
		//TODO 에러 발생함. 연결 종료.
		UE_LOG(LogTemp, Warning, TEXT("TLS Error!"));
		break;
	}
}

void TLSSession::HandshakeSend()
{
	uint32 PendingDataSize = SslRef->GetWBioPendingSize();
	while (PendingDataSize > 0)
	{
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(PendingDataSize);

		uint32 readLen = SslRef->ReadWBio(sendBuffer->GetBuffer(), PendingDataSize);
		sendBuffer->OnWrite(readLen);


		while (readLen > 0)
		{
			int32 SentLen = 0;
			if (false == Socket->Send(sendBuffer->GetBuffer(), sendBuffer->GetDataLen(), SentLen))
			{
				//TODO send 실패 처리
				UE_LOG(LogTemp, Warning, TEXT("TLS HandshakeSend Error!"));
				return;
			}
			sendBuffer->OnRead(SentLen);
			readLen -= SentLen;
			PendingDataSize -= SentLen;
		}
	}
}

void TLSSession::HandshakeRecv()
{
	int32 ReadLen = 0;
	RecvBufferRef recvBuffer = MakeShared<RecvBuffer>(0x2000); //8KB
	if (false == Socket->Recv(recvBuffer->WritePos(), recvBuffer->FreeSize(), ReadLen))
	{
		//TODO recv 실패 처리
		return;
	}
	if (ReadLen <= 0)
	{
		//TODO recv 실패 처리
		return;
	}
	recvBuffer->OnWrite(ReadLen);
	uint32 WriteLen = SslRef->WriteRBio(recvBuffer->ReadPos(), recvBuffer->DataSize());
	TLSConnect();
}
