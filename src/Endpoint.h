#pragma once

class Endpoint {
public:
	Endpoint( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port ) {
		m_address = unsigned int(a) | (unsigned int(b) << 8) | (unsigned int(c) << 16) | (unsigned int(d) << 24);
		m_port = port;
	}

	bool operator ==( const Endpoint & rhs ) const {
		return (m_address == rhs.m_address) && (m_port == rhs.m_port);
	}

	bool operator <( const Endpoint & rhs ) const {
		if( m_address == rhs.m_address ) {
			return m_port < rhs.m_port;
		}
		return m_address < rhs.m_address;
	}

protected:
	unsigned int	m_address;
	unsigned short	m_port;
};