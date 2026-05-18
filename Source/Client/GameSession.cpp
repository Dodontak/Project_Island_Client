#include "GameSession.h"

#include "Protocol.pb.h"
#include "ServerPacketHandler.h"

GameSession::GameSession(FString IpAddress, uint32 Port, SSL_CTX* CTX) : TLSSession(IpAddress, Port, CTX)
{
}

GameSession::~GameSession()
{
}

void GameSession::RequestMyCharacterList()
{
	Protocol::GC_CHARACTER_LIST Pkt;
	
	SendPacket(ServerPacketHandler::MakeSendBuffer(Pkt));
}
