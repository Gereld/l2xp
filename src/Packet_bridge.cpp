#include "Packet_bridge.h"
#include "l2/PacketExtractor.h"

Packet_bridge::Packet_bridge( const std::shared_ptr<l2::PacketExtractor> & pHandler ) 
	: m_pHandler( pHandler )
{
}

void Packet_bridge::process( const std::shared_ptr<net::tcp_ip_packet> & packet ) {
	if( packet->data_length() > 0 ) {
		m_pHandler->add( const_cast<unsigned char *>( packet->data_ptr() ), packet->data_length() );
	}
}
