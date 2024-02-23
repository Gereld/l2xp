#pragma once

#include "IPacket_processor.h"

namespace net {

class Packet_filter : public IPacket_processor {
public:
	Packet_filter( const std::shared_ptr<IPacket_processor> & pNext );
	~Packet_filter();

	virtual void process( const std::shared_ptr<tcp_ip_packet> & packet );

protected:
	std::shared_ptr<IPacket_processor> m_pNext;
};

} // namespace net