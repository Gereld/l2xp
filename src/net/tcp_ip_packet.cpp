#include "tcp_ip_packet.h"

namespace net {

tcp_ip_packet::tcp_ip_packet( const unsigned char * data, unsigned int len ) 
	: m_data( data, data + len )
{
}

tcp_ip_packet::~tcp_ip_packet() {
}

} // namespace net