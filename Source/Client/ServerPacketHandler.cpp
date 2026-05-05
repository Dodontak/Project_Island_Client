#include "ServerPacketHandler.h"
#include "Session.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool	Handle_INVALID(DeferredFunc& outFunc, PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

void	Handle_S_LOGIN(const PacketSessionRef& session, const Protocol::S_LOGIN& pkt)
{
}

void	Handle_S_ENTER_ROOM(const PacketSessionRef& session, const Protocol::S_ENTER_ROOM& pkt)
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

void	Handle_S_CHAT(const PacketSessionRef& session, const Protocol::S_CHAT& pkt)
{
	Protocol::C_CHAT Response;
	
	Response.set_msg(pkt.msg());
	// session->SendPacket(ServerPacketHandler::MakeSendBuffer(Response));
}
