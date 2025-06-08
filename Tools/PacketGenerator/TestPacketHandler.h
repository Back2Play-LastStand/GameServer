#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = function<bool(Session*, BYTE*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : unsigned short
{
	PKT_REQ_ENTER = 1000,
	PKT_RES_ENTER = 1001,
	PKT_REQ_ENTER_ROOM = 1002,
	PKT_RES_ENTER_ROOM = 1003,
	PKT_REQ_LEAVE = 1004,
	PKT_RES_LEAVE = 1005,
	PKT_REQ_RESPAWN = 1006,
	PKT_RES_SPAWN = 1007,
	PKT_RES_SPAWN_ALL = 1008,
	PKT_RES_DESPAWN = 1009,
	PKT_RES_CHANGE_HP = 1010,
	PKT_RES_DIE = 1011,
	PKT_REQ_MOVE = 1012,
	PKT_RES_MOVE = 1013,
	PKT_RES_SPAWN_MONSTER = 1014,
	PKT_RES_MOVE_MONSTER = 1015,
	PKT_REQ_ATTACK_OBJECT = 1016,
	PKT_RES_ATTACK_OBJECT = 1017,
};

bool Handle_INVALID(Session* session, BYTE* buffer, int len);
bool Handle_REQ_ENTER(Session* session, Protocol::REQ_ENTER& pkt);
bool Handle_REQ_ENTER_ROOM(Session* session, Protocol::REQ_ENTER_ROOM& pkt);
bool Handle_REQ_LEAVE(Session* session, Protocol::REQ_LEAVE& pkt);
bool Handle_REQ_RESPAWN(Session* session, Protocol::REQ_RESPAWN& pkt);
bool Handle_REQ_MOVE(Session* session, Protocol::REQ_MOVE& pkt);
bool Handle_REQ_ATTACK_OBJECT(Session* session, Protocol::REQ_ATTACK_OBJECT& pkt);

class TestPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_REQ_ENTER] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_ENTER>(Handle_REQ_ENTER, session, buffer, len); };
		GPacketHandler[PKT_REQ_ENTER_ROOM] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_ENTER_ROOM>(Handle_REQ_ENTER_ROOM, session, buffer, len); };
		GPacketHandler[PKT_REQ_LEAVE] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_LEAVE>(Handle_REQ_LEAVE, session, buffer, len); };
		GPacketHandler[PKT_REQ_RESPAWN] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_RESPAWN>(Handle_REQ_RESPAWN, session, buffer, len); };
		GPacketHandler[PKT_REQ_MOVE] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_MOVE>(Handle_REQ_MOVE, session, buffer, len); };
		GPacketHandler[PKT_REQ_ATTACK_OBJECT] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_ATTACK_OBJECT>(Handle_REQ_ATTACK_OBJECT, session, buffer, len); };
	}
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_RES_ENTER); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_ENTER_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_RES_ENTER_ROOM); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_LEAVE& pkt) { return MakeSendBuffer(pkt, PKT_RES_LEAVE); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_RES_SPAWN); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_SPAWN_ALL& pkt) { return MakeSendBuffer(pkt, PKT_RES_SPAWN_ALL); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_RES_DESPAWN); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_CHANGE_HP& pkt) { return MakeSendBuffer(pkt, PKT_RES_CHANGE_HP); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_DIE& pkt) { return MakeSendBuffer(pkt, PKT_RES_DIE); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_RES_MOVE); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_SPAWN_MONSTER& pkt) { return MakeSendBuffer(pkt, PKT_RES_SPAWN_MONSTER); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_MOVE_MONSTER& pkt) { return MakeSendBuffer(pkt, PKT_RES_MOVE_MONSTER); }
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::RES_ATTACK_OBJECT& pkt) { return MakeSendBuffer(pkt, PKT_RES_ATTACK_OBJECT); }

	static bool HandlePacket(Session* session, BYTE* buffer, int len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, Session* session, BYTE* buffer, int len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static shared_ptr<vector<char>> MakeSendBuffer(T& pkt, unsigned short id)
	{
		const unsigned short dataSize = static_cast<unsigned short>(pkt.ByteSizeLong());
		const unsigned short packetSize = dataSize + sizeof(PacketHeader);

		shared_ptr<vector<char>> sendBuffer = MakeShared<vector<char>>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->data());
		header->size = packetSize;
		header->id = id;
		assert(pkt.SerializeToArray(sendBuffer->data() + 4, dataSize));

		return sendBuffer;
	}
};
