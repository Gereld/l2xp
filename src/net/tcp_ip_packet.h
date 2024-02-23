#pragma once

#include <memory>
#include <vector>

#include "ip_header_helper.h"
#include "tcp_header_helper.h"

namespace net {

class tcp_ip_packet {
public:
	tcp_ip_packet( const unsigned char * data, unsigned int len );
	~tcp_ip_packet();

	const unsigned char * ptr() const {
		return m_data.data();
	}

	unsigned int length() const {
		return m_data.size();
	}

	ip_header_helper ip() const {
		return ip_header_helper( m_data.data() );
	}

	tcp_header_helper tcp() const {
		return tcp_header_helper( ip().data() );
	}

	const unsigned char * data_ptr() const {
		return tcp().data();
	}

	unsigned int data_length() const {
		return ip().total_length() - ip().header_length() - tcp().header_length();
	}

protected:
	std::vector<unsigned char> m_data;
};

} // namespace net
