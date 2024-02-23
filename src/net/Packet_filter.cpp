#include "Packet_filter.h"

namespace net {

Packet_filter::Packet_filter( const std::shared_ptr<IPacket_processor> & pNext ) 
	: m_pNext( pNext )
{
}

Packet_filter::~Packet_filter() {
}

void Packet_filter::process( const std::shared_ptr<tcp_ip_packet> & packet ) {
	if( m_pNext ) {
		m_pNext->process( packet );
	}
}

} // namespace net