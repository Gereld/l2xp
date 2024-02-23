#pragma once

#include "headers.h"

namespace net {

class ip_header_helper {
public:
	ip_header_helper( const unsigned char * pData ) 
		: m_pData( pData )
	{
	}

	unsigned char version() const {
		return ( ih()->ver_ihl & 0xF0 ) >> 4;
	}

	unsigned int header_length() const { 
		return ( ih()->ver_ihl & 0xF ) * 4;
	}

	unsigned char protocol() const {
		return ih()->proto;
	}

	unsigned short total_length() const {
		return ntohs( ih()->tlen );
	}

	unsigned short identification() const {
		return ih()->identification;
	}

	unsigned char flags() const {
		return (ntohs( ih()->flags_fo ) >> 13) & 0x07;
	}

	unsigned short fragment_offset() const {
		return ntohs( ih()->flags_fo ) & ~(0x07 << 13);
	}

	const ip_address & source_address() const {
		return ih()->saddr;
	}

	const ip_address & destination_address() const {
		return ih()->daddr;
	}

	const unsigned char * ptr() const {
		return m_pData;
	}

	const unsigned char * data() const {
		return ptr() + header_length();
	}

protected:
	const unsigned char * m_pData;
	const ip_header * ih() const { return reinterpret_cast<const ip_header *>( m_pData ); }
};

} // namespace net