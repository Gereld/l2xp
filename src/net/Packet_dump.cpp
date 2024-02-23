#include "Packet_dump.h"

#include <Gx/io/FileOutputStream.h>

namespace net {

Packet_dump::Packet_dump( const std::shared_ptr<Gx::io::OutputStream> & out, const std::shared_ptr<IPacket_processor> & pNext ) 
	: Packet_filter( pNext )
{
	if( out ) {
		m_dos = std::make_shared<Gx::io::DataOutputStream>( out );
	}
}

Packet_dump::~Packet_dump() {
}

void Packet_dump::process( const std::shared_ptr<tcp_ip_packet> & packet ) {
	if( m_dos ) {
		m_dos->WriteInt( packet->length() );
		m_dos->Write( reinterpret_cast<const char *>( packet->ptr() ), packet->length() );
		m_dos->Flush();
	}
	__super::process( packet );
}

} // namespace net