#include "ServerPacketHandler.h"
#include "PacketSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(DeferredFunc& outFunc, PacketSessionRef& session, BYTE* buffer, int32 len)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid Packet")));
	return false;
}

void Handle_S_LOGIN(const PacketSessionRef& session, const Protocol::S_LOGIN& pkt)
{
}

void Handle_S_ENTER_ROOM(const PacketSessionRef& session, const Protocol::S_ENTER_ROOM& pkt)
{
}

void Handle_S_LEAVE_ROOM(const PacketSessionRef& session, const Protocol::S_LEAVE_ROOM& pkt)
{
}

void Handle_S_SPAWN(const PacketSessionRef& session, const Protocol::S_SPAWN& pkt)
{
}

void Handle_S_DESPAWN(const PacketSessionRef& session, const Protocol::S_DESPAWN& pkt)
{
}

void Handle_S_CHAT(const PacketSessionRef& session, const Protocol::S_CHAT& pkt)
{
	Protocol::C_CHAT Response;
	FString Result = FString(UTF8_TO_TCHAR(pkt.msg().c_str()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
	                                 FString::Printf(TEXT("Recv From Server: %hs"), pkt.msg().c_str()));
	Response.set_msg(pkt.msg());
	session->SendPacket(ServerPacketHandler::MakeSendBuffer(Response));
}
