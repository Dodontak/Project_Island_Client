#include "AuthSession.h"

#include "GameSession.h"
#include "ServerPacketHandler.h"

AuthSession::AuthSession(FString IpAddress, uint32 Port, SSL_CTX* CTX) : TLSSession(IpAddress, Port, CTX)
{
}

AuthSession::~AuthSession()
{
}

void AuthSession::LoginToAuthServer(FString Id, FString Password)
{
	Protocol::AC_LOGIN Pkt;
	Pkt.set_nickname(TCHAR_TO_UTF8(*Id));
	Pkt.set_password(TCHAR_TO_UTF8(*Password));

	SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void AuthSession::SignUpValidCheck(bool SkipEmail, FString Email, FString Id, FString Password)
{
	Protocol::AC_SIGNUP Pkt;
	SetEmailAddress(Email);

	Pkt.set_skip_email(SkipEmail);
	if (SkipEmail == false)
		Pkt.set_email(TCHAR_TO_UTF8(*Email));
	Pkt.set_nickname(TCHAR_TO_UTF8(*Id));
	Pkt.set_password(TCHAR_TO_UTF8(*Password));
	SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void AuthSession::RequestVerifyCode()
{
	Protocol::AC_VERIFY_MAIL_REQ Pkt;

	Pkt.set_temp_id(TCHAR_TO_UTF8(*TempId_));
	SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void AuthSession::SignUpVerifyEmailCode(FString Code)
{
	Protocol::AC_VERIFY_EMAIL_CODE Pkt;

	Pkt.set_temp_id(TCHAR_TO_UTF8(*TempId_));
	Pkt.set_verify_code(TCHAR_TO_UTF8(*Code));
	SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}

void AuthSession::LoginToGameServer(TSharedPtr<GameSession> GameServer)
{
	Protocol::GC_LOGIN Pkt;

	Pkt.set_jwt(TCHAR_TO_UTF8(*JWT_));
	GameServer->SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}
