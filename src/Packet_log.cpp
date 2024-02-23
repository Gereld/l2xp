#include "Packet_log.h"
#include "net/tools.h"

Packet_log::Packet_log( const std::shared_ptr<net::IPacket_processor> & pNext ) 
	: Packet_filter( pNext )
{
}

void Packet_log::process( const std::shared_ptr<net::tcp_ip_packet> & packet ) {
	DBG_LOG( std::endl );
	DBG_LOG( "tlen: " << packet->ip().total_length() << " id: " << packet->ip().identification() << " flags: " << (int) packet->ip().flags() << " " << net::ip_flags_to_str( packet->ip().flags() ) << " offset: "  << packet->ip().fragment_offset() << std::endl );
	DBG_LOG( "sport: " << packet->tcp().source_port() << " dport: " << packet->tcp().destination_port() << " seq: " << packet->tcp().sequence_number() << " ack: " << packet->tcp().acknowlegdment_number() << " flags: " << (int) packet->tcp().flags() << " " << net::tcp_flags_to_str( packet->tcp().flags() ) << std::endl );
	DBG_LOG( "dlen: " << packet->data_length() << std::endl );
	__super::process( packet );
}
