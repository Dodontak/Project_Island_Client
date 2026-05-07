// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkWorker.h"
#include "PacketSession.h"
#include "Sockets.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "PacketSession.h"
#include "SslObject.h"

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 RecvWorker                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session)
	: SslRef(nullptr), DecBuffer_(BUFFER_SIZE), Socket(Socket), sessionRef(Session)
{
}

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, SslObjectRef Ssl)
	: SslRef(Ssl), DecBuffer_(BUFFER_SIZE), Socket(Socket), sessionRef(Session)
{
}

RecvWorker::~RecvWorker()
{
}

bool RecvWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Recv Thread Init"));
	return true;
}

uint32 RecvWorker::Run()
{
	while (Running)
	{
		Recv();
	}
	return 0;
}

void RecvWorker::Exit()
{
}

void RecvWorker::StartThread()
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

void RecvWorker::Destroy()
{
	Running = false;
}

bool RecvWorker::Recv()
{
	int32 BytesReceived = 0;
	RecvBuffer& EncBuffer = GetEncBuffer();
	RecvBuffer& DecBuffer = GetDecBuffer();

	if (false == Socket->Recv(EncBuffer.WritePos(), EncBuffer.FreeSize(), BytesReceived))
	{
		//TODO Recv 실패 시 처리
		return false;
	}
	if (BytesReceived == 0)
	{
		//TODO Disconnect 처리
		return false;
	}
	EncBuffer.OnWrite(BytesReceived);

	bool Repeat = true;
	while (Repeat)
	{
		switch (Decrypt(EncBuffer, DecBuffer))
		{
		case 0: // 성공. 복호화 할 데이터 더 있을 수 있음. 반복해서 복호화 시도.
			break;
		case 1: // 복호화 데이터 부족
			Repeat = false;
			break;
		case 2: // 상대가 shutdown. shutdown 호출 가능
			//TODO shutdown 정상종료
			Repeat = false;
			break;
		default: // ret = 3 에러. shutdown 호출 불가능.
			//TODO Disconnect
			Repeat = false;
			break;
		}
	}
	uint32 DecLen = OnRecv(DecBuffer.ReadPos(), DecBuffer.DataSize());
	DecBuffer.OnRead(DecLen);

	EncBuffer.Clean();
	DecBuffer.Clean();

	return true;
}

uint32 RecvWorker::OnRecv(BYTE* buffer, uint32 len)
{
	uint32 processLen = 0;
	while (true)
	{
		uint32 dataLen = len - processLen;
		if (dataLen < sizeof(PacketHeader))
			break;

		PacketHeader* header = reinterpret_cast<PacketHeader*>(&buffer[processLen]);

		if (dataLen < header->size)
			break;

		if (TSharedPtr<PacketSession> session = sessionRef.Pin())
		{
			TArray<uint8> Packet;
			Packet.Append(&buffer[processLen], header->size);
			session->RecvPacketQueue.Enqueue(Packet);
			processLen += header->size;
		}
	}
	return processLen;
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                               TLSRecvWorker                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/
TLSRecvWorker::TLSRecvWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, SslObjectRef Ssl)
	: RecvWorker(Socket, Session, Ssl), EncBuffer_(BUFFER_SIZE)
{
}

// enc버퍼의 암호문을 복호화 해서 dec버퍼에 넣는 함수.
// 리턴 0 성공. 1 복호화 데이터 부족 2 shutdown, 3 에러
uint8 TLSRecvWorker::Decrypt(RecvBuffer& EncBuffer, RecvBuffer& DecBuffer)
{
	uint32 wlen = SslRef->WriteRBio(EncBuffer.ReadPos(), EncBuffer.DataSize());
	EncBuffer.OnRead(wlen);

	size_t recvSize;
	SslStatus status = SslRef->Read(DecBuffer.WritePos(), DecBuffer.FreeSize(), &recvSize);
	switch (status)
	{
	case SslStatus::Ok:
		DecBuffer.OnWrite(recvSize);
		return 0;
	case SslStatus::WantRead: //복호화 하기에 데이터 부족함.
		return 1;
	case SslStatus::Shutdown: //상대가 shutdown함. shutdown 호출 가능.
		return 2;
	default: // 에러 발생. shutdown 호출 불가.
		return 3;
	}
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 SendWorker                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/
SendWorker::SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session)
	: Socket(Socket), SessionRef(Session), SslRef(nullptr)
{
}

SendWorker::SendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, TSharedPtr<SslObject> Ssl)
	: Socket(Socket), SessionRef(Session), SslRef(Ssl)
{
}

SendWorker::~SendWorker()
{
}

bool SendWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Send Thread Init")));

	return true;
}

uint32 SendWorker::Run()
{
	while (Running)
	{
		SendBufferRef SendBuffer;

		if (TSharedPtr<PacketSession> Session = SessionRef.Pin())
		{
			if (Session->SendDeque.TryPopFirst(SendBuffer))
			{
				SendPacket(SendBuffer);
			}
		}
		// Sleep?
	}
	return 0;
}

void SendWorker::Exit()
{
}

bool SendWorker::SendPacket(SendBufferRef sendBuffer)
{
	SendBufferRef EncBuffer;
	{
		bool isSuccess = Encrypt(sendBuffer, EncBuffer);
		if (isSuccess == false)
			return false;
	}

	if (SendDesiredBytes(EncBuffer->GetBuffer(), EncBuffer->GetDataLen()) == false)
		return false;
	return true;
}

bool SendWorker::Encrypt(SendBufferRef& decBuffer, SendBufferRef& encBuffer)
{
	encBuffer = decBuffer;
	return true;
}

void SendWorker::StartThread()
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

void SendWorker::Destroy()
{
	Running = false;
}

bool SendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		int32 BytesSent = 0;
		if (Socket->Send(Buffer, Size, BytesSent) == false)
			return false;
		Size -= BytesSent;
		Buffer += BytesSent;
	}
	return true;
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                               TLSSendWorker                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/
TLSSendWorker::TLSSendWorker(FSocket* Socket, TSharedPtr<PacketSession> Session, SslObjectRef Ssl)
	: SendWorker(Socket, Session, Ssl)
{
}

bool TLSSendWorker::Encrypt(SendBufferRef& decBuffer, SendBufferRef& encBuffer)
{
	size_t writtenLen;
	SslStatus status = SslRef->Write(decBuffer->GetBuffer(), decBuffer->GetDataLen(), &writtenLen);
	if (status == SslStatus::Fail)
		return false;
	uint32 pendingSize = SslRef->GetWBioPendingSize();
	if (pendingSize == 0)
		return false;
	SendBufferRef sendBuffer = MakeShared<SendBuffer>(pendingSize);
	uint32 rlen = SslRef->ReadWBio(sendBuffer->GetBuffer(), pendingSize);
	sendBuffer->OnWrite(rlen);
	encBuffer = sendBuffer;
	return true;
}
