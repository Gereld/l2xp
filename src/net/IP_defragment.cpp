#include "IP_defragment.h"

namespace net {

IP_defragment::IP_defragment( const std::shared_ptr<IPacket_processor> & pNext ) 
	: Packet_filter( pNext )
{
}

void IP_defragment::process( const std::shared_ptr<tcp_ip_packet> & packet ) {
	if( (packet->ip().flags() & IP_MORE_FRAGMENT) || (packet->ip().fragment_offset() != 0) ) {
		DBG_LOG( "Detected more fragments to come" << std::endl );

		__super::process( packet );
	}
	else {
		__super::process( packet );
	}
}

} // namespace net