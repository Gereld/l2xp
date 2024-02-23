#pragma once

#include "net/Packet_filter.h"

class Packet_log : public net::Packet_filter {
public:
	Packet_log( const std::shared_ptr<net::IPacket_processor> & pNext );

	virtual void process( const std::shared_ptr<net::tcp_ip_packet> & packet );
};