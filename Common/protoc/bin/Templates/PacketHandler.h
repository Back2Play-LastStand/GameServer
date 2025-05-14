#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = function<bool(Session*, BYTE*, int)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : unsigned short
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

bool Handle_INVALID(Session* session, BYTE* buffer, int len);

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(Session* session, Protocol::{{pkt.name}}& pkt);
{%- endfor %}

class {{output}}
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](Session* session, BYTE* buffer, int len) {return HandlePacket<Protocol::{{pkt.name}}>(Handle_{{pkt.name}}, session, buffer, len); };
{%- endfor %}
	}

{%- for pkt in parser.send_pkt %}
	static shared_ptr<vector<char>> MakeSendBuffer(Protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}); }
{%- endfor %}

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

