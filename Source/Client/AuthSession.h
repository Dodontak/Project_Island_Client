#pragma once
#include "PacketSession.h"

class GameSession;

class CLIENT_API AuthSession : public TLSSession
{
public:
	AuthSession(FString IpAddress, uint32 Port, SSL_CTX* CTX);
	virtual ~AuthSession();

public:
	void LoginToAuthServer(FString Id, FString Password);
	void SignUpValidCheck(bool SkipEmail, FString Email, FString Id, FString Password);
	void RequestVerifyCode();
	void SignUpVerifyEmailCode(FString Code);
	void LoginToGameServer(TSharedPtr<GameSession> GameServer);
	
	void SetJWT(FString JWT) { JWT_ = JWT; }
	FString GetJWT() { return JWT_; }

	void SetTempId(FString TempId) { TempId_ = TempId; }
	FString GetTempId() { return TempId_; }
	
	void SetEmailAddress(FString EmailAddress) { EmailAddress_ = EmailAddress; }
	FString GetEmailAddress() { return EmailAddress_; }

private:
	FString JWT_;
	FString TempId_;
	FString EmailAddress_;
};
