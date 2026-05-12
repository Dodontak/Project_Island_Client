#include "GameSession.h"

GameSession::GameSession(FString IpAddress, uint32 Port, SSL_CTX* CTX) : TLSSession(IpAddress, Port, CTX)
{
}

GameSession::~GameSession()
{
}
