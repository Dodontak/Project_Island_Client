#pragma once
#include "PacketSession.h"

class GameSession : public TLSSession
{
public:
	GameSession(FString IpAddress, uint32 Port, SSL_CTX* CTX);
	virtual ~GameSession();

public:
	FString JWT_;
};
