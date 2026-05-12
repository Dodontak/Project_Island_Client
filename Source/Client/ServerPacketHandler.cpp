#include "ServerPacketHandler.h"
#include "PacketSession.h"
#include "ClientGameInstance.h"
#include "AuthSession.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(DeferredFunc& outFunc, PacketSessionRef& session, BYTE* buffer, int32 len)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid Packet")));
	return false;
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 GameServer                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void Handle_GS_LOGIN(const PacketSessionRef& session, const Protocol::GS_LOGIN& pkt)
{
	UE_LOG(LogTemp, Warning, TEXT("Handle_GS_LOGIN"));

	if (pkt.success())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameServer Login Success!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameServer Login Failed!"));
	}
}

void Handle_GS_ENTER_ROOM(const PacketSessionRef& session, const Protocol::GS_ENTER_ROOM& pkt)
{
}

void Handle_GS_LEAVE_ROOM(const PacketSessionRef& session, const Protocol::GS_LEAVE_ROOM& pkt)
{
}

void Handle_GS_SPAWN(const PacketSessionRef& session, const Protocol::GS_SPAWN& pkt)
{
}

void Handle_GS_DESPAWN(const PacketSessionRef& session, const Protocol::GS_DESPAWN& pkt)
{
}

void Handle_GS_CHAT(const PacketSessionRef& session, const Protocol::GS_CHAT& pkt)
{
	Protocol::GC_CHAT Response;
	FString Result = FString(UTF8_TO_TCHAR(pkt.msg().c_str()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
	                                 FString::Printf(TEXT("Recv From Server: %hs"), pkt.msg().c_str()));
	Response.set_msg(pkt.msg());
	session->SendPacket(ServerPacketHandler::MakeSendBuffer(Response));
}

/*----------------------------------------------------------------------------*\
|                                                                              |
|                                 AuthServer                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void Handle_AS_SIGNUP(const PacketSessionRef& session, const Protocol::AS_SIGNUP& pkt)
{
	UE_LOG(LogTemp, Warning, TEXT("Handle_AS_SIGNUP"));

	if (pkt.success())
	{
		TSharedPtr<AuthSession> AuthServer = StaticCastSharedPtr<AuthSession>(session);

		AuthServer->SetTempId(FString(pkt.temp_id().c_str()));
		AuthServer->RequestVerifyCode();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("reason : %hs"), pkt.reason().c_str());
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->OnSignUpValidCheck.Broadcast(false, "", pkt.reason().c_str());
		}
	}
}

void Handle_AS_VERIFY_MAIL_REQ(const PacketSessionRef& session, const Protocol::AS_VERIFY_MAIL_REQ& pkt)
{
	UE_LOG(LogTemp, Warning, TEXT("Handle_AS_VERIFY_MAIL_REQ"));

	TSharedPtr<AuthSession> AuthServer = StaticCastSharedPtr<AuthSession>(session);
	if (pkt.success())
	{
		// TODO SignUp 위젯 닫고 이메일 인증 위젯 열기
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->OnSignUpValidCheck.Broadcast(true, AuthServer->GetEmailAddress(), "");
		}
	}
	else
	{
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->OnSignUpValidCheck.Broadcast(false, AuthServer->GetEmailAddress(), pkt.reason().c_str());
		}
	}
}

void Handle_AS_VERIFY_EMAIL_CODE(const PacketSessionRef& session, const Protocol::AS_VERIFY_EMAIL_CODE& pkt)
{
	UE_LOG(LogTemp, Warning, TEXT("Handle_AS_VERIFY_EMAIL_CODE"));
	TSharedPtr<AuthSession> AuthServer = StaticCastSharedPtr<AuthSession>(session);
	if (pkt.success())
	{
		// TODO SignUp 위젯 닫고 이메일 인증 위젯 열기
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->OnSignUpVerifyCode.Broadcast(true, "");
		}
	}
	else
	{
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->OnSignUpVerifyCode.Broadcast(false, pkt.reason().c_str());
		}
	}
}

void Handle_AS_LOGIN(const PacketSessionRef& session, const Protocol::AS_LOGIN& pkt)
{
	UE_LOG(LogTemp, Warning, TEXT("Handle_AS_LOGIN"));
	if (pkt.success())
	{
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->GameServerSession = MakeShared<GameSession>("127.0.0.1", 7777, GI->Ctx);
			bool Success = GI->GameServerSession->ConnectToGameServer();

			if (Success)
			{
				TSharedPtr<AuthSession> AuthServer = StaticCastSharedPtr<AuthSession>(session);
				AuthServer->SetJWT(pkt.token().c_str());
				AuthServer->LoginToGameServer(StaticCastSharedPtr<GameSession>(GI->GameServerSession));
			}
			else
			{
				GI->OnLoginToGameServer.Broadcast(false, pkt.reason().c_str());
			}
		}
	}
	else
	{
		if (UClientGameInstance* GI = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		{
			GI->OnLoginToGameServer.Broadcast(false, pkt.reason().c_str());
		}
	}
}
