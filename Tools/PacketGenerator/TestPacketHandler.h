#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = function<bool(Session*, BYTE*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : unsigned short
{
	PKT_REQ_ENTER = 1000,
	PKT_RES_ENTER = 1001,
};

bool Handle_INVALID(Session* session, BYTE* buffer, int len);
bool Handle_REQ_ENTER(Session* session, Protocol::REQ_ENTER& pkt);

class TestPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_REQ_ENTER] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::REQ_ENTER>(Handle_REQ_ENTER, session, buffer, len); };
	}
	static vector<char> MakeSendBuffer(Protocol::RES_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_RES_ENTER); }

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
	static vector<char> MakeSendBuffer(T& pkt, unsigned short id)
	{
		const unsigned short dataSize = static_cast<unsigned short>(pkt.ByteSizeLong());
		const unsigned short packetSize = dataSize + sizeof(PacketHeader);

		vector<char> sendBuffer = make_shared<vector<char>>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer.data() + 4, dataSize);
		header->size = packetSize;
		header->id = id;
		assert(pkt.SerializeToArray(&header[1], dataSize));

		return sendBuffer;
	}
};
