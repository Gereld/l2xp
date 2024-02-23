#pragma once

#include "tcp_ip_packet.h"

#include <memory>

namespace net {

class IPacket_processor {
public:
	virtual void process( const std::shared_ptr<tcp_ip_packet> & packet ) = 0;
};

} // namespace net