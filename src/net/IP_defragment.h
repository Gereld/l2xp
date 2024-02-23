#pragma once

#include "Packet_filter.h"

namespace net {

class IP_defragment : public Packet_filter {
public:
	IP_defragment( const std::shared_ptr<IPacket_processor> & pNext );

	void process( const std::shared_ptr<tcp_ip_packet> & packet );
};

} // namespace net