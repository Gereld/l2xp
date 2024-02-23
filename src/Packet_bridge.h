#pragma once

#include "l2/PacketExtractor.h"
#include "net/IPacket_processor.h"

class Packet_bridge : public net::IPacket_processor {
public:
	Packet_bridge( const std::shared_ptr<l2::PacketExtractor> & pHandler );

	void process( const std::shared_ptr<net::tcp_ip_packet> & packet );

protected:
	std::shared_ptr<l2::PacketExtractor> m_pHandler;
};