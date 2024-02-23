#pragma once

#include "Packet_filter.h"
#include <Gx/io/DataOutputStream.h>

namespace net {

class Packet_dump : public Packet_filter {
public:
	Packet_dump( const std::shared_ptr<Gx::io::OutputStream> & out, const std::shared_ptr<IPacket_processor> & pNext );
	~Packet_dump();

	virtual void process( const std::shared_ptr<tcp_ip_packet> & packet );

protected:
	std::shared_ptr<Gx::io::DataOutputStream> m_dos;
};

} // namespace net