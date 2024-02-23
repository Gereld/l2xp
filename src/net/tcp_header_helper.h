#pragma once

#include "headers.h"

namespace net {

class tcp_header_helper {
public:
	tcp_header_helper( const unsigned char * pData ) 
		: m_pData( pData )
	{
	}

	unsigned short source_port() const {
		return ntohs( th()->sport );
	}

	unsigned short destination_port() const {
		return ntohs( th()->dport );
	}

	unsigned int sequence_number() const {
		return ntohl( th()->seqnum );
	}

	unsigned int acknowlegdment_number() const {
		return ntohl( th()->acknum );
	}

	unsigned int header_length() const {
		return (th()->hlen & 0xF0) >> 2;
	}

	unsigned char flags() const {
		return th()->flags;
	}

	const unsigned char * ptr() const {
		return m_pData;
	}

	const unsigned char * data() const {
		return ptr() + header_length();
	}

protected:
	const unsigned char * m_pData;
	const tcp_header * th() const { return reinterpret_cast<const tcp_header *>( m_pData ); }
};

} // namespace net
